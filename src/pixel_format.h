//
// Created by Alex on 10.09.2019.
//

#ifndef SH_PIXEL_FORMAT_H
#define SH_PIXEL_FORMAT_H

#include <ostream>

#include "real.h"

namespace sh {
    template<class T>
    struct RGBStruct {
        T r;
        T g;
        T b;

        RGBStruct() = default;

        RGBStruct(T v) : r(v), g(v), b(v) {}

        template<class F>
        RGBStruct(RGBStruct<F> v) : r((F)v.r), g((F)v.g), b((F)v.b) {}

        RGBStruct(T r, T g, T b) : r(r), g(g), b(b) {}

        RGBStruct<T> operator+(const RGBStruct<T> &v) const{
            return {r + v.r, g + v.g, b + v.b};
        }

        RGBStruct<T> operator+=(const RGBStruct<T> &v)  {
            r += v.r;
            g += v.g;
            b += v.b;
            return *this;
        }

        RGBStruct<T> operator-(const RGBStruct<T> &v) const {
            return {r - v.r, g - v.g, b - v.b};
        }

        RGBStruct<T> operator*(const RGBStruct<T> &v) const {
            return {r * v.r, g * v.g, b * v.b};
        }

        RGBStruct<T> operator*(T m) const {
            return RGBStruct<T>(r * m, g * m, b * m);
        }

        RGBStruct<T> operator/(const RGBStruct<T> &v) {
            return {r / v.r, g / v.g, b / v.b};
        }

        RGBStruct<T> operator/(T m) {
            return {r / m, g / m, b / m};
        }
    };

    template<class T>
    std::ostream &operator<<(std::ostream &stream, const RGBStruct<T> &item) {
        stream << "{" << (int) item.r << ", " << (int) item.g << ", " << (int) item.b << "}";
        return stream;
    }


    template<class T>
    struct RGBAStruct {
        T r;
        T g;
        T b;
        T a;

        RGBAStruct() = default;

        template<class F>
        RGBAStruct(RGBAStruct<F> v) : r((F)v.r), g((F)v.g), b((F)v.b), a((F)v.a) {}

        RGBAStruct(T v) : r(v), g(v), b(v), a(v) {}

        RGBAStruct(T r, T g, T b, T a) : r(r), g(g), b(b), a(a) {}

        RGBAStruct<T> operator+(const RGBAStruct<T> &v) {
            return {r + v.r, g + v.g, b + v.b, a + v.a};
        }

        RGBAStruct<T> operator+=(const RGBAStruct<T> &v) {
            r += v.r;
            g += v.g;
            b += v.b;
            a += v.a;
            return *this;
        }

        RGBAStruct<T> operator-(const RGBAStruct<T> &v) {
            return {r - v.r, g - v.g, b - v.b, a - v.a};
        }

        RGBAStruct<T> operator*(const RGBAStruct<T> &v) {
            return {r * v.r, g * v.g, b * v.b, a * v.a};
        }

        RGBAStruct<T> operator*(T m) {
            return {r * m, g * m, b * m, a * m};
        }

        RGBAStruct<T> operator/(const RGBAStruct<T> &v) {
            return {r / v.r, g / v.g, b / v.b, a / v.a};
        }

        RGBAStruct<T> operator/(T m) {
            return {r / m, g / m, b / m, a / m};
        }
    };

    template<class T>
    std::ostream &operator<<(std::ostream &stream, const RGBAStruct<T> &item) {
        stream << "{" << item.r << ", " << item.g << ", " << item.b << ", " << item.a << "}";
        return stream;
    }

    using RGB = RGBStruct<real>;
    using RGBA= RGBAStruct<real>;
    using RGBF = RGBStruct<float>;
    using RGBAF= RGBAStruct<float>;
}
#endif //SH_PIXEL_FORMAT_H
