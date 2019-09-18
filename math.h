//
// Created by Alex on 15.09.2019.
//

#ifndef SH_MATH_H
#define SH_MATH_H

#include <cmath>
#include <functional>
#include <glm/glm.hpp>

namespace SH {
    namespace math {

        const float PI = 3.14159265358979f;

        template<class T>
        using PolarFunction = std::function<T(float, float)>;

        float fact(float a) {
            if (a <= 1.0f) {
                return 1.0;
            }
            return a * fact(a - 1.0f);
        }

        /**
         * Convert spherical coordinates to cartesian (x, y, z)
         * @param phi
         * @param tetta
         * @return
         */
        glm::vec3 sphericalToCartesian(float phi, float tetta) {
            float sinTetta = sinf(tetta);
            return glm::vec3(sinTetta * cosf(phi), sinTetta * sinf(phi), cosf(tetta));
        }

        /**
         * Convert cartesian coordinates (x, y, z) to spherical(phi, tetta)
         * @param p
         * @return
         */
        glm::vec2 catesianToSpherical(const glm::vec3 &p) {
            return glm::vec2(atan2f(p.y, p.x), acosf(p.z));
        }


        /**
         * Evaluate an Associated Legendre Polynomial P(l,m,x) at x
         * @param l band index [0, R]
         * @param m index ogf polynomial inside of band [0, l]
         * @param x
         * @return
         */
        float P(int l, int m, float x) {

            float pmm = 1.0;
            if (m > 0) {
                float somx2 = sqrtf((1.0f - x) * (1.0f + x));
                float fact = 1.0;
                for (int i = 1; i <= m; i++) {
                    pmm *= (-fact) * somx2;
                    fact += 2.0;
                }
            }
            if (l == m) return pmm;
            float pmmp1 = x * (2.0f * m + 1.0f) * pmm;
            if (l == m + 1) return pmmp1;
            float pll = 0.0;
            for (int ll = m + 2; ll <= l; ++ll) {
                pll = ((2.0f * ll - 1.0f) * x * pmmp1 - (ll + m - 1.0f) * pmm) / (ll - m);
                pmm = pmmp1;
                pmmp1 = pll;
            }
            return pll;
        }


        /**
         * Renormalisation constant for SH function
         * @param l
         * @param m
         * @return
         */
        float K(int l, int m) {
            float temp = ((2.0f * l + 1.0f) * fact(l - m)) / (4.0f * PI * fact(l + m));
            return sqrtf(temp);
        }

        /**
         * Return a point sample of a Spherical Harmonic basis function
         * @param l is the band, range [0..N]
         * @param m  in the range [-l..l]
         * @param phi in the range [0..2*Pi]
         * @param tetta in the range [0..Pi]
         * @return
         */
        float y(int l, int m, float phi, float tetta) {
            const float sqrt2 = sqrtf(2.0f);
            if (m == 0) {
                return K(l, 0) * P(l, m, cosf(tetta));
            } else if (m > 0) {
                return sqrt2 * K(l, m) * cosf(m * phi) * P(l, m, cosf(tetta));
            } else {
                return sqrt2 * K(l, -m) * sinf(-m * phi) * P(l, -m, cosf(tetta));
            }
        }

        float radicalInverse_VdC(uint32_t bits) {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
        }

        glm::vec2 hammersley2d(uint16_t i, uint16_t N) {
            return glm::vec2(float(i) / float(N), radicalInverse_VdC(i));
        }

        glm::vec2 sampleSphere(float ex, float ey) {
            return glm::vec2(2.0f * math::PI * ey, 2.0 * acosf(sqrtf(1.0f - ex)));
        }

    }
}
#endif //SH_MATH_H
