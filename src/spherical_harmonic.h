//
// Created by Alex on 14.09.2019.
//

#ifndef SH_SPHERICALHARMONIC_H
#define SH_SPHERICALHARMONIC_H

#include <vector>
#include <inttypes.h>
#include <ostream>
#include <map>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>

#include "CubeMap.h"
#include "sampling.h"
#include "math.h"
#include "CubeMapPolarFunction.h"

namespace sh {

    template<class F>
    using ShCoefficients = std::vector<F>;

    template<class F>
    uint16_t order(const ShCoefficients<F> &coefficients) {
        return (uint16_t) (sqrt(coefficients.size()) - 1u);
    }

    template<class T>
    T estimateSpherical(const math::PolarFunction<T> &polarFunction, int l, int m, uint16_t divisions = 64) {
        float dPhi = 2.0f * math::PI / divisions, dTetta = 2.0f * math::PI / divisions;
        float phi = 0.0f, tetta;
        T estimation(0.0f);
        for (int i = 0; i < divisions; i++) {
            tetta = 0.0f;
            for (int j = 0; j < divisions / 2; j++) {
                float y = math::y(l, m, phi, tetta);
                estimation += polarFunction(phi, tetta) * y * sinf(tetta) * dPhi * dTetta;
                tetta += dTetta;
            }
            phi += dPhi;
        }
        return estimation;
    }

    template<class T>
    T estimateMonteCarlo(const math::PolarFunction<T> &polarFunction, int l, int m, uint16_t samples = 512) {
        const float factor = 4.0f * math::PI / samples;
        T estimation(0.0f);
        for (uint16_t i = 0; i < samples; i++) {
            auto e = math::hammersley2d(i, samples);
            auto angles = math::sampleSphere(e.x, e.y);
            float y = math::y(l, m, angles.x, angles.y);
            estimation += polarFunction(angles.x, angles.y) * y;
        }
        return estimation * factor;
    }

    float projectedArea(float s, float t) {
        return std::atan2(s * t, std::sqrt(s * s + t * t + 1.0f));
    }

    float solidAngle(float s, float t, float ds, float dt) {
        s = std::abs(s);
        t = std::abs(t);
        float C = projectedArea(s, t);
        float A = projectedArea(s - ds, t - dt);
        float B = projectedArea(s - ds, t);
        float D = projectedArea(s, t - dt);
        return A - B + C - D;
    }

    template<class T>
    T estimateCubeMap(const std::shared_ptr<CubeMap<T>> &cubemap, int l, int m) {
        using namespace std;
        using namespace glm;
        using namespace math;

        const map<CubeMapFaceEnum, mat3> transformLookup = {
                {CubeMapFaceEnum::PositiveX, mat3(-axis::z, axis::y, -axis::x)},
                {CubeMapFaceEnum::NegativeX, mat3(axis::z, axis::y, axis::x)},
                {CubeMapFaceEnum::PositiveY, mat3(axis::x, -axis::z, -axis::y)},
                {CubeMapFaceEnum::NegativeY, mat3(axis::x, axis::z, axis::y)},
                {CubeMapFaceEnum::PositiveZ, mat3(axis::x, axis::y, axis::z)},
                {CubeMapFaceEnum::NegativeZ, mat3(-axis::x, axis::y, -axis::z)},
        };

        T estimation(0.0f);
        const int w = cubemap->getWidth(), h = cubemap->getHeight();
        float dt = 2.0f / h, ds = 2.0f / w;
        float s, t = -1.0f + dt * 0.5f;
        for (auto &p: transformLookup) {
            const auto transform = p.second;
            for (int i = 0; i < h; i++) {
                s = -1.0f + ds * 0.5f;
                for (int j = 0; j < w; j++) {
                    vec3 r = transform * normalize(vec3(s, t, -1.0f));
                    vec2 angles = math::cartesianToSpherical(r);
                    T sample = sampleCubemap<T>(*cubemap, r, InterpolationMethod::Nearest);
                    float dw = solidAngle(s, t, ds, dt);
                    float y = math::y(l, m, angles.x, angles.y);
                    estimation += sample * y * dw;
                    s += ds;
                }
                t += dt;
            }
        }
        return estimation;
    }

    template<class F>
    ShCoefficients<F> encode(const std::shared_ptr<CubeMap<F> > &cubeMap, uint16_t order, SamplingMethod method,
            uint16_t samples, InterpolationMethod filtering) {

        ShCoefficients<F> coefficients((order + 1u) * (order + 1u));
        if (method == SamplingMethod::MonteCarlo) {
            CubeMapPolarFunction<F> polarFunction(cubeMap, filtering);
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateMonteCarlo<F>(polarFunction, l, m, samples);
                }
            }
        } else if (method == SamplingMethod::Sphere) {
            CubeMapPolarFunction<F> polarFunction(cubeMap, filtering);
            const auto divisions = (uint16_t) sqrtf(2.0f * samples);
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateSpherical<F>(polarFunction, l, m, divisions);
                }
            }
        } else {
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateCubeMap<F>(cubeMap, l, m);
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
    template<class F>
    F decode(const ShCoefficients<F> &coefficients, float phi, float tetta) {
        const auto n = order(coefficients);
        F decoded(0);
        for (int l = 0; l <= n; l++) {
            for (int m = -l; m <= l; m++) {
                int index = l * (l + 1) + m;
                F c = coefficients[index];
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
    template<class F>
    std::shared_ptr<CubeMap<F>> decode(const ShCoefficients<F> &coefficients, int size) {
        using namespace std;
        using namespace glm;
        using namespace math;

        const map<CubeMapFaceEnum, mat3> transformLookup = {
                {CubeMapFaceEnum::PositiveX, mat3(-axis::z, axis::y, -axis::x)},
                {CubeMapFaceEnum::NegativeX, mat3(axis::z, axis::y, axis::x)},
                {CubeMapFaceEnum::PositiveY, mat3(axis::x, -axis::z, -axis::y)},
                {CubeMapFaceEnum::NegativeY, mat3(axis::x, axis::z, axis::y)},
                {CubeMapFaceEnum::PositiveZ, mat3(axis::x, axis::y, axis::z)},
                {CubeMapFaceEnum::NegativeZ, mat3(-axis::x, axis::y, -axis::z)},
        };

        float dt = 2.0f / size, ds = 2.0f / size;
        map<CubeMapFaceEnum, shared_ptr<PixelArray<F>>> faces;
        for (auto &item : transformLookup) {
            const auto face = item.first;
            const auto transform = item.second;

            auto bitmap = make_shared<PixelArray<F>>(new F[size * size], size, size);

            float s, t = -1.0f + dt * 0.5f;
            for (int y = 0; y < size; y++) {
                s = -1.0f + ds * 0.5f;
                for (int x = 0; x < size; x++) {
                    vec3 r = transform * normalize(vec3(s, t, -1.0f));
                    vec2 angles = math::cartesianToSpherical(r);
                    (*bitmap)[y][x] = decode<F>(coefficients, angles.x, angles.y);
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


    template<class F>
    F product(const ShCoefficients<F> &a, const ShCoefficients<F> &b) {
        F dot(0);
        for (int i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
        }
        return dot;
    }
}
#endif //SH_SPHERICALHARMONIC_H
