//
// Created by Alex on 10.09.2019.
//

#ifndef SH_SAMPLING_H
#define SH_SAMPLING_H

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/epsilon.hpp>

#include "real.h"
#include "pixel_format.h"
#include "PixelArray.h"
#include "CubeMap.h"
#include "math.h"

namespace sh {

    enum class SamplingMethod {
        Sphere,
        MonteCarlo,
        Cubemap
    };

    enum class InterpolationMethod {
        Nearest,
        Bilinear
    };

    template<class T>
    struct Texel {
        Texel() = default;
        T value;
        real x = 0;
        real y = 0;
    };


    template<class T>
    T linear(const Texel<T> &q11, const Texel<T> &q12, const Texel<T> &q21, const Texel<T> &q22, real x, real y) {
        real x1 = q11.x, x2 = q21.x, y1 = q11.y, y2 = q12.y;

        T a1 = q11.value * (y2 - y) + q12.value * (y - y1);
        T a2 = q21.value * (y2 - y) + q22.value * (y - y1);

        T dot = a1 * (x2 - x) + a2 * (x - x1);

        return dot / (x2 - x1) / (y2 - y1);
    }

    template<class T>
    T nearest(const Texel<T> &q11, const Texel<T> &q12, const Texel<T> &q21, const Texel<T> &q22, real x, real y) {
        real x1 = q11.x, x2 = q21.x, y1 = q11.y, y2 = q12.y;
        real ds = abs((y2 - y1) * (x2 - x1)) * 0.25;
        real s11 = abs((y - y1) * (x - x1));
        real s12 = abs((y - y2) * (x - x1));
        real s21 = abs((y - y1) * (x - x2));

        if (s11 < ds) {
            return q11.value;
        } else if (s12 < ds) {
            return q12.value;
        } else if (s21 < ds) {
            return q21.value;
        } else {
            return q22.value;
        }
    }

    /**
     * Sample 2d texture bitmap
     * @tparam T pixel format
     * @param bitmap image to sample from
     * @param uv normalized texture coordinates [0.0f, 1.0f]
     * @param filtering interpolation method
     * @return
     */
    template<class T>
    T sampleBitmap(PixelArray <T> &bitmap, const vec2 &uv, InterpolationMethod filtering) {
        real w = std::max(0, bitmap.getWidth() - 1), h = std::max(0, bitmap.getHeight() - 1);
        real x = uv.x * w, y = uv.y * h;
        real x1 = std::min(w - 1.0, std::floor(x)), y1 = std::min(h - 1.0, std::floor(y));
        real x2 = std::min(w, x1 + 1.0), y2 = std::min(h, y1 + 1.0);

        Texel<T> q11;
        q11.x = x1;
        q11.y = y1;
        q11.value = T(bitmap[q11.y][q11.x]);

        Texel<T> q12;
        q12.x = x1;
        q12.y = y2;
        q12.value = T(bitmap[q12.y][q12.x]);

        Texel<T> q21;
        q21.x = x2;
        q21.y = y1;
        q21.value = T(bitmap[q21.y][q21.x]);

        Texel<T> q22;
        q22.x = x2;
        q22.y = y2;
        q22.value = T(bitmap[q22.y][q22.x]);

        if (filtering == InterpolationMethod::Nearest) {
            return nearest(q11, q12, q21, q22, x, y);
        } else {
            return linear(q11, q12, q21, q22, x, y);
        }
    }

    /**
     * @tparam T
     * @param cubemap
     * @param dir
     * @param filtering
     * @return
     */
    template<class T>
    T sampleCubemap(CubeMap <T> &cubemap, const vec3 &dir, InterpolationMethod filtering) {

        // 1. find cubemap face
        // 2. project dir to face
        // 3. find 2d texture map pixel
        // 4. sample pixel using provided interpolation

        // 1.
        vec3 r = normalize(dir);
        real maximum = std::max(std::max(std::fabs(r.x), std::fabs(r.y)), std::fabs(r.z));
        real xx = r.x / maximum;
        real yy = r.y / maximum;
        real zz = r.z / maximum;


        CubeMapFaceEnum face;
        if (xx == 1.0) {
            face = CubeMapFaceEnum::PositiveX;
        } else if (xx == -1.0) {
            face = CubeMapFaceEnum::NegativeX;
        } else if (yy == 1.0) {
            face = CubeMapFaceEnum::PositiveY;
        } else if (yy == -1.0) {
            face = CubeMapFaceEnum::NegativeY;
        } else if (zz == 1.0) {
            face = CubeMapFaceEnum::PositiveZ;
        } else {
            face = CubeMapFaceEnum::NegativeZ;
        }

        // 2.
        vec3 projection;
        if (face == CubeMapFaceEnum::PositiveX || face == CubeMapFaceEnum::NegativeX) {
            projection.x = xx * 0.5;
            projection.y = projection.x / r.x * r.y;
            projection.z = projection.x / r.x * r.z;
        } else if (face == CubeMapFaceEnum::PositiveY || face == CubeMapFaceEnum::NegativeY) {
            projection.y = yy * 0.5;
            projection.x = projection.y / r.y * r.x;
            projection.z = projection.y / r.y * r.z;
        } else {
            projection.z = zz * 0.5;
            projection.x = projection.z / r.z * r.x;
            projection.y = projection.z / r.z * r.y;
        }

        // 3.
        vec2 uv;
        if (face == CubeMapFaceEnum::PositiveX) {
            uv = vec2(-projection.z, projection.y) + vec2(0.5);
        } else if (face == CubeMapFaceEnum::NegativeX) {
            uv = vec2(projection.z, projection.y) + vec2(0.5);
        } else if (face == CubeMapFaceEnum::PositiveY) {
            uv = vec2(projection.x, -projection.z) + vec2(0.5);
        } else if (face == CubeMapFaceEnum::NegativeY) {
            uv = vec2(projection.x, projection.z) + vec2(0.5);
        } else if (face == CubeMapFaceEnum::PositiveZ) {
            uv = vec2(projection.x, projection.y) + vec2(0.5);
        } else {
            uv = vec2(-projection.x, projection.y) + vec2(0.5);
        }

        // 4.
        return sampleBitmap<T>(*(cubemap[face]), uv, filtering);
    }


}
#endif //SH_SAMPLING_H
