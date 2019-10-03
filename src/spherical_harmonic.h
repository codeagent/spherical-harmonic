//
// Created by Alex on 14.09.2019.
//

#ifndef SH_SPHERICALHARMONIC_H
#define SH_SPHERICALHARMONIC_H

#include <vector>
#include <inttypes.h>
#include <ostream>
#include <iostream>
#include <map>
#include <cmath>

#include "real.h"
#include "CubeMap.h"
#include "sampling.h"
#include "shmath.h"
#include "CubeMapPolarFunction.h"

namespace sh {

    template<class R>
    using ShCoefficients = std::vector<R>;

    template<class R>
    uint16_t order(const ShCoefficients<R> &coefficients) {
        return (uint16_t) (std::sqrt(coefficients.size()) - 1u);
    }

    template<class R>
    R estimateSpherical(const math::PolarFunction<R> &polarFunction, int l, int m, uint16_t divisions = 64) {
        real dPhi = math::PI2 / divisions, dTetta = math::PI2 / divisions;
        real phi = 0, tetta;
        R estimation(0);
        for (int i = 0; i < divisions; i++) {
            tetta = 0;
            for (int j = 0; j < divisions / 2; j++) {
                real y = math::y(l, m, phi, tetta);
                estimation += polarFunction(phi, tetta) * y * std::sin(tetta) * dPhi * dTetta;
                tetta += dTetta;
            }
            phi += dPhi;
        }
        return estimation;
    }

    template<class R>
    R estimateMonteCarlo(const math::PolarFunction<R> &polarFunction, int l, int m, uint16_t samples = 512) {
        const real factor = math::PI4 / samples;
        R estimation(0.0f);
        for (uint16_t i = 0; i < samples; i++) {
            auto e = math::hammersley2d(i, samples);
            auto angles = math::sampleSphere(e.x, e.y);
            real y = math::y(l, m, angles.x, angles.y);
            estimation += polarFunction(angles.x, angles.y) * y;
        }
        return estimation * factor;
    }

    real projectedArea(real s, real t) {
        return std::atan2(s * t, std::sqrt(s * s + t * t + 1));
    }

    real solidAngle(real s, real t, real ds, real dt) {
        ds = ds * 0.5;
        dt = dt * 0.5;
        real C = projectedArea(s + ds, t + dt);
        real A = projectedArea(s - ds, t - dt);
        real B = projectedArea(s + ds, t - dt);
        real D = projectedArea(s - ds, t + dt);
        return A - B + C - D;
    }

    template<class R, class F>
    R estimateCubeMap(const std::shared_ptr<CubeMap<F>> &cubemap, int l, int m) {
        using namespace std;
        using namespace math;

        const map<CubeMapFaceEnum, mat3> transformLookup = {
                {CubeMapFaceEnum::PositiveX, mat3(-axis::z, axis::y, -axis::x)},
                {CubeMapFaceEnum::NegativeX, mat3(axis::z, axis::y, axis::x)},
                {CubeMapFaceEnum::PositiveY, mat3(axis::x, -axis::z, -axis::y)},
                {CubeMapFaceEnum::NegativeY, mat3(axis::x, axis::z, axis::y)},
                {CubeMapFaceEnum::PositiveZ, mat3(axis::x, axis::y, -axis::z)},
                {CubeMapFaceEnum::NegativeZ, mat3(-axis::x, axis::y, axis::z)}
        };

        R estimation(0);
        const int w = cubemap->getWidth(), h = cubemap->getHeight();
        real dt = 2.0 / h, ds = 2.0 / w;
        for (auto &p: transformLookup) {
            real s, t = -1 + dt * 0.5;
            const auto transform = p.second;
            for (int i = 0; i < h; i++) {
                s = -1 + ds * 0.5;
                for (int j = 0; j < w; j++) {
                    vec3 r = transform * normalize(vec3(s, t, -1));
                    vec2 angles = math::cartesianToSpherical(r);
                    R sample = R(sampleCubemap<F>(*cubemap, r, InterpolationMethod::Nearest));
                    real dw = solidAngle(std::abs(s), std::abs(t), ds, dt);
                    real y = math::y(l, m, angles.x, angles.y);
                    estimation += sample * y * dw;
                    s += ds;
                }
                t += dt;
            }
        }
        return estimation;
    }

    template<class R, class F>
    ShCoefficients<R> encode(const std::shared_ptr<CubeMap<F>> &cubeMap, uint16_t order, SamplingMethod method,
            uint16_t samples, InterpolationMethod filtering) {

        ShCoefficients<R> coefficients((order + 1u) * (order + 1u));
        if (method == SamplingMethod::MonteCarlo) {
            CubeMapPolarFunction<R, F> polarFunction(cubeMap, filtering);
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateMonteCarlo<R>(polarFunction, l, m, samples);
                }
            }
        } else if (method == SamplingMethod::Sphere) {
            CubeMapPolarFunction<R, F> polarFunction(cubeMap, filtering);
            const auto divisions = (uint16_t) std::sqrt(2.0 * samples);
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateSpherical<R>(polarFunction, l, m, divisions);
                }
            }
        } else {
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateCubeMap<R>(cubeMap, l, m);
                }
            }
        }

        return coefficients;
    }


    /**
     * Get decoded value by parameterized spherical coordinates
     * @param coefficients
     * @param phi
     * @param tetta
     * @return
     */
    template<class R>
    R decode(const ShCoefficients<R> &coefficients, real phi, real tetta) {
        const auto n = order(coefficients);
        R decoded(0);
        for (int l = 0; l <= n; l++) {
            for (int m = -l; m <= l; m++) {
                int index = l * (l + 1) + m;
                R c = coefficients[index];
                decoded += c * math::y(l, m, phi, tetta);
            }
        }
        return decoded;
    }

    /**
     * Convert encoded signal into cubemap
     * @tparam F
     * @param coefficients
     * @param size
     * @return
     */
    template<class R, class F>
    std::shared_ptr<CubeMap<F>> decode(const ShCoefficients<R> &coefficients, int size) {
        using namespace std;
        using namespace glm;
        using namespace math;

        const map<CubeMapFaceEnum, mat3> transformLookup = {
                {CubeMapFaceEnum::PositiveX, mat3(-axis::z, axis::y, -axis::x)},
                {CubeMapFaceEnum::NegativeX, mat3(axis::z, axis::y, axis::x)},
                {CubeMapFaceEnum::PositiveY, mat3(axis::x, -axis::z, -axis::y)},
                {CubeMapFaceEnum::NegativeY, mat3(axis::x, axis::z, axis::y)},
                {CubeMapFaceEnum::PositiveZ, mat3(axis::x, axis::y, -axis::z)},
                {CubeMapFaceEnum::NegativeZ, mat3(-axis::x, axis::y, axis::z)}
        };

        real dt = 2.0 / size, ds = 2.0 / size;
        map<CubeMapFaceEnum, shared_ptr<PixelArray<F>>> faces;
        for (auto &item : transformLookup) {
            const auto face = item.first;
            const auto transform = item.second;

            auto bitmap = make_shared<PixelArray<F>>(new F[size * size], size, size);

            real s, t = -1 + dt * 0.5;
            for (int y = 0; y < size; y++) {
                s = -1 + ds * 0.5;
                for (int x = 0; x < size; x++) {
                    vec3 r = transform * normalize(vec3(s, t, -1));
                    vec2 angles = math::cartesianToSpherical(r);
                    const auto v = decode<R>(coefficients, angles.x, angles.y);
                    (*bitmap)[y][x] = F(v);
                    s += ds;
                }
                t += dt;
            }
            faces[face] = bitmap;
        }
        return make_shared<CubeMap<F>>(
                faces[CubeMapFaceEnum::PositiveX],
                faces[CubeMapFaceEnum::NegativeX],
                faces[CubeMapFaceEnum::PositiveY],
                faces[CubeMapFaceEnum::NegativeY],
                faces[CubeMapFaceEnum::PositiveZ],
                faces[CubeMapFaceEnum::NegativeZ]);
    }

}
#endif //SH_SPHERICALHARMONIC_H
