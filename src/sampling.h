//
// Created by Alex on 10.09.2019.
//

#ifndef SH_SAMPLING_H
#define SH_SAMPLING_H

#include <glm/glm.hpp>
#include <cmath>

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
        float x = 0.0f;
        float y = 0.0f;
    };


    template<class T>
    T linear(const Texel<T> &q11, const Texel<T> &q12, const Texel<T> &q21, const Texel<T> &q22, float x,
             float y) {
        const auto eps = 1.0e-3f;
        float x1 = q11.x, x2 = q21.x, y1 = q11.y, y2 = q12.y;

        T a1 = q11.value * (y2 - y) + q12.value * (y - y1);
        T a2 = q21.value * (y2 - y) + q22.value * (y - y1);

        T dot = a1 * (x2 - x) + a2 * (x - x1);

        return dot / (x2 == x1 ? eps : x2 - x1) / (y2 == y1 ? eps : y2 - y1);
    }

    template<class T>
    T nearest(const Texel<T> &q11, const Texel<T> &q12, const Texel<T> &q21, const Texel<T> &q22, float x,
              float y) {
        float x1 = q11.x, x2 = q21.x, y1 = q11.y, y2 = q12.y;
        float ds = abs((y2 - y1) * (x2 - x1)) * 0.25f;
        float s11 = abs((y - y1) * (x - x1));
        float s12 = abs((y - y2) * (x - x1));
        float s21 = abs((y - y1) * (x - x2));

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
     * @param uv normalized texture coordinates (0.0f, 1.0f)
     * @param filtering interpolation method
     * @return
     */
    template<class T>
    T sampleBitmap(PixelArray <T> &bitmap, const glm::vec2 &uv, InterpolationMethod filtering) {
        auto w = bitmap.getWidth(), h = bitmap.getHeight();
        float x = uv.x * w, y = uv.y * h;
        const float d = 1.0e-3f;
        Texel<T> q11;
        q11.x = fmaxf(0.0f, floorf(x));
        q11.y = fmaxf(0.0f, floorf(y));
        q11.value = bitmap[q11.y][q11.x];

        Texel<T> q12;
        q12.x = fmaxf(0.0f, floorf(x));
        q12.y = fminf(h - 1.0f, ceilf(y + d));
        q12.value = bitmap[q12.y][q12.x];

        Texel<T> q21;
        q21.x = fminf(w - 1.0f, ceilf(x + d));
        q21.y = fmaxf(0.0f, floorf(y));
        q21.value = bitmap[q21.y][q21.x];

        Texel<T> q22;
        q22.x = fminf(w - 1.0f, ceilf(x + d));
        q22.y = fminf(h - 1.0f, ceilf(y + d));
        q22.value = bitmap[q22.y][q22.x];

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
    T sampleCubemap(CubeMap <T> &cubemap, const glm::vec3 &dir, InterpolationMethod filtering) {

        // 1. find cubemap face
        // 2. project dir to face
        // 3. find 2d texture map pixel
        // 4. sample pixel using provided interpolation

        // 1.
        glm::vec3 r = glm::normalize(dir);
        float maximum = fmaxf(fmaxf(abs(r.x), abs(r.y)), abs(r.z));
        float xx = r.x / maximum;
        float yy = r.y / maximum;
        float zz = r.z / maximum;


        CubeMapFaceEnum face;
        if (xx == 1.0f) {
            face = CubeMapFaceEnum::PositiveX;
        } else if (xx == -1.0f) {
            face = CubeMapFaceEnum::NegativeX;
        } else if (yy == 1.0f) {
            face = CubeMapFaceEnum::PositiveY;
        } else if (yy == -1.0f) {
            face = CubeMapFaceEnum::NegativeY;
        } else if (zz == 1.0f) {
            face = CubeMapFaceEnum::PositiveZ;
        } else {
            face = CubeMapFaceEnum::NegativeZ;
        }

        // 2.
        glm::vec3 projection;
        if (face == CubeMapFaceEnum::PositiveX || face == CubeMapFaceEnum::NegativeX) {
            projection.x = xx * 0.5f;
            projection.y = projection.x / r.x * r.y;
            projection.z = projection.x / r.x * r.z;
        } else if (face == CubeMapFaceEnum::PositiveY || face == CubeMapFaceEnum::NegativeY) {
            projection.y = yy * 0.5f;
            projection.x = projection.y / r.y * r.x;
            projection.z = projection.y / r.y * r.z;
        } else {
            projection.z = zz * 0.5f;
            projection.x = projection.z / r.z * r.x;
            projection.y = projection.z / r.z * r.y;
        }

        // 3.
        glm::vec2 uv;
        if (face == CubeMapFaceEnum::PositiveX) {
            uv = glm::vec2(-projection.z, projection.y) + glm::vec2(0.5f);
        } else if (face == CubeMapFaceEnum::NegativeX) {
            uv = glm::vec2(projection.z, projection.y) + glm::vec2(0.5f);
        } else if (face == CubeMapFaceEnum::PositiveY) {
            uv = glm::vec2(projection.x, -projection.z) + glm::vec2(0.5f);
        } else if (face == CubeMapFaceEnum::NegativeY) {
            uv = glm::vec2(projection.x, projection.z) + glm::vec2(0.5f);
        } else if (face == CubeMapFaceEnum::PositiveZ) {
            uv = glm::vec2(projection.x, projection.y) + glm::vec2(0.5f);
        } else {
            uv = glm::vec2(-projection.x, projection.y) + glm::vec2(0.5f);
        }

        // 4.
        return sampleBitmap<T>(*(cubemap[face]), uv, filtering);
    }


}
#endif //SH_SAMPLING_H
