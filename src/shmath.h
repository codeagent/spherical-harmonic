//
// Created by Alex on 15.09.2019.
//

#ifndef SH_MATH_H
#define SH_MATH_H

#include <cmath>
#include <functional>

#include "real.h"

namespace sh {
    namespace math {

        const real PI = (real)3.14159265358979;
        const real SQRT2 = (real)1.41421356237309;
        const real PI4 = 4 * PI;
        const real PI2 = 2 * PI;

        template<class T>
        using PolarFunction = std::function<T(real, real)>;

        namespace axis {
            const vec3 x(1, 0, 0);
            const vec3 y(0, 1, 0);
            const vec3 z(0, 0, 1);
        }

        real fact(real a) {
            if (a <= 1) {
                return 1;
            }
            return a * fact(a - 1.0);
        }

        /**
         * Convert spherical coordinates to cartesian (x, y, z) (OpenGL space)
         * @param phi
         * @param tetta
         * @return
         */
        vec3 sphericalToCartesian(real phi, real tetta) {
            real sinTetta = std::sin(tetta);
            return vec3(sinTetta * std::sin(phi), std::cos(tetta), sinTetta * std::cos(phi));
        }

        /**
        * Convert cartesian coordinates (OpenGL x, y, z) to spherical(phi, tetta)
        * @param p
        * @return
        */
        vec2 cartesianToSpherical(const vec3 &p) {
            return vec2(std::atan2(p.x, p.z), std::acos(p.y));
        }

        /**
         * Evaluate an Associated Legendre Polynomial P(l,m,x) at x
         * @param l band index [0, R]
         * @param m index ogf polynomial inside of band [0, l]
         * @param x
         * @return
         */
        real P(int l, int m, real x) {

            real pmm = 1;
            if (m > 0) {
                real somx2 = std::sqrt((1 - x) * (1 + x));
                real fact = 1;
                for (int i = 1; i <= m; i++) {
                    pmm *= (-fact) * somx2;
                    fact += 2;
                }
            }
            if (l == m) return pmm;
            real pmmp1 = x * (2 * m + 1) * pmm;
            if (l == m + 1) return pmmp1;
            real pll = 0.0;
            for (int ll = m + 2; ll <= l; ++ll) {
                pll = ((2 * ll - 1) * x * pmmp1 - (ll + m - 1) * pmm) / (ll - m);
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
        real K(int l, int m) {
            real temp = ((2 * l + 1) * fact(l - m)) / (PI4 * fact(l + m));
            return std::sqrt(temp);
        }

        template<class T>
        bool equal(T a, T b , const T epsilon = std::numeric_limits<T>::epsilon()) {
            return std::abs(b - a) < epsilon;
        }

        /**
         * Return a point sample of a Spherical Harmonic basis function
         * @param l is the band, range [0..N]
         * @param m  in the range [-l..l]
         * @param phi in the range [0..2*Pi]
         * @param tetta in the range [0..Pi]
         * @return
         */
        real y(int l, int m, real phi, real tetta) {
            double result;
            if (m == 0) {
                result = K(l, 0) * P(l, m, std::cos(tetta));
            } else if (m > 0) {
                result = SQRT2 * K(l, m) * std::cos(m * phi) * P(l, m, std::cos(tetta));
            } else {
                result = SQRT2 * K(l, -m) * std::sin(-m * phi) * P(l, -m, std::cos(tetta));
            }
            if(std::isnan(result)) {
                return 0;
            }
            return result;
        }

        float radicalInverse_VdC(uint32_t bits) {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10f; // / 0x100000000
        }

        vec2 hammersley2d(uint16_t i, uint16_t N) {
            return vec2(real(i) / real(N), radicalInverse_VdC(i));
        }

        vec2 sampleSphere(real ex, real ey) {
            return vec2(PI2 * ey, 2 * std::acos(std::sqrt(1 - ex)));
        }

    }
}
#endif //SH_MATH_H
