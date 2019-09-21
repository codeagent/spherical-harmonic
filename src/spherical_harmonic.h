//
// Created by Alex on 14.09.2019.
//

#ifndef SH_SPHERICALHARMONIC_H
#define SH_SPHERICALHARMONIC_H

#include <vector>
#include <inttypes.h>
#include <ostream>

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

    template<class F>
    ShCoefficients<F> encode(uint16_t order, const math::PolarFunction<F> &polarFunction, SamplingMethod method,
                             uint16_t samples) {

        ShCoefficients<F> coefficients((order + 1u) * (order + 1u));
        if (method == SamplingMethod::MonteCarlo) {
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateMonteCarlo(polarFunction, l, m, samples);
                }
            }
        } else {
            const auto divisions = (uint16_t) sqrtf(2.0f * samples);
            for (int l = 0; l <= order; l++) {
                for (int m = -l; m <= l; m++) {
                    int index = l * (l + 1) + m;
                    coefficients[index] = estimateSpherical(polarFunction, l, m, divisions);
                }
            }
        }
        return coefficients;
    }


    template<class F>
    F decode(const ShCoefficients<F> &coefficients, float phi, float tetta) {
        const auto n = order(coefficients);
        F decoded(0);
        for (int l = 0; l <= n; l++) {
            for (int m = -l; m <= l; m++) {
                int index = l * (l + 1) + m;
                F c = c[index];
                decoded += c * math::y(l, m, phi, tetta);
            }
        }
        return decoded;
    }


    template<class F>
    F product(const ShCoefficients<F> &a, const ShCoefficients<F> &b) {
        F dot(0);
        for (int i = 0; i < a.size(); i++) {
            dot += a[i] * b[i];
        }
        return dot;
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
}
#endif //SH_SPHERICALHARMONIC_H
