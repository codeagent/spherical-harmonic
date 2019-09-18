//
// Created by Alex on 14.09.2019.
//

#ifndef SH_SPHERICALHARMONIC_H
#define SH_SPHERICALHARMONIC_H

#include <array>
#include <vector>
#include <inttypes.h>
#include <ostream>

#include "CubeMap.h"
#include "sampling.h"
#include "math.h"
#include "CubeMapPolarFunction.h"

namespace SH {

    template<class F, int N>
    using ShCoefficients = std::array<F, (N + 1) * (N + 1)>;


    template<class F, int N>
    ShCoefficients<F, N> encode(const math::PolarFunction<F> &polarFunction,
                                SamplingMethod method,
                                uint16_t samples) {
        ShCoefficients<F, N> coefficients;

        for (int l = 0; l <= N; l++) {
            for (int m = -l; m <= l; l++) {
                int index = l * (l + 1) + m;
                if (method == SamplingMethod::MonteCarlo) {
                    coefficients[index] = estimateMonteCarlo(polarFunction, l, m, samples);
                } else {
                    coefficients[index] = estimateSpherical(polarFunction, l, m, (uint16_t) sqrtf(2.0f * samples));
                }
            }
        }
        return coefficients;
    }

    template<class F, int N>
    uint16_t order(const ShCoefficients<F, N> &coefficients) {
        return (uint16_t) sqrt(coefficients.size());
    }


    template<class F, int N>
    F decode(const ShCoefficients<F, N> &coefficients, float phi, float tetta) {
        F decoded(0);
        for (int l = 0; l <= N; l++) {
            for (int m = -l; m <= l; l++) {
                int index = l * (l + 1) + m;
                F c = c[index];
                decoded += c * math::y(l, m, phi, tetta);
            }
        }
        return decoded;
    }


    template<class F, int N>
    F product(const ShCoefficients<F, N> &a, const ShCoefficients<F, N> &b) {
        F dot(0);
        for (int i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
        }
        return dot;
    }

    template<class F, int N>
    std::ostream &operator<<(std::ostream &stream, const ShCoefficients<F, N> &h) {

        stream << "{";
        stream << "\"order\": " << order(h) << ", ";
        stream << "\"channels\": ";
        stream << "{ ";
        // @todo:
        return stream;
    }

    template<class T>
    T estimateSpherical(const math::PolarFunction<T> &polarFunction, int l, int m, uint16_t divisions = 64) {
        float dPhi = 2.0f * math::PI / divisions, dTetta = math::PI / divisions;
        float phi = 0.0f, tetta = 0.0f;
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
}
#endif //SH_SPHERICALHARMONIC_H
