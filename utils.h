//
// Created by Alex on 12.09.2019.
//

#ifndef SH_UTILS_H
#define SH_UTILS_H

#include <memory>
#include <inttypes.h>
#include <string>

#include <stb_image.h>

#include "PixelArray.h"
#include "pixel_format.h"
#include "CubeMap.h"
#include "spherical_harmonic.h"

namespace SH {
    using namespace std;

    shared_ptr<PixelArray<RGB>> loadPixelArrayRgb(const string &path) {
        int width, height, channels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 3);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGB>>((RGB *) data, width, height);
    }

    shared_ptr<PixelArray<RGBA>> loadPixelArrayRgba(const string &path) {
        int width, height, channels;
        unsigned char *data = stbi_load(path.c_str(), &width, &height, &channels, 4);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGBA>>((RGBA *) data, width, height);
    }

    shared_ptr<PixelArray<RGBFloat>> loadPixelArrayRgbFloat(const string &path) {
        int width, height, channels;
        float *data = stbi_loadf(path.c_str(), &width, &height, &channels, 3);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGBFloat>>((RGBFloat *) data, width, height);
    }

    shared_ptr<PixelArray<RGBAFloat>> loadPixelArrayRgbaFloat(const string &path) {
        int width, height, channels;
        float *data = stbi_loadf(path.c_str(), &width, &height, &channels, 4);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGBAFloat>>((RGBAFloat *) data, width, height);
    }

    shared_ptr<CubeMap<RGB>> loadCubemapRgb(
            const string &px,
            const string &nx,
            const string &py,
            const string &ny,
            const string &pz,
            const string &nz
    ) {
        const auto pxBmp = loadPixelArrayRgb(px);
        const auto nxBmp = loadPixelArrayRgb(nx);
        const auto pyBmp = loadPixelArrayRgb(py);
        const auto nyBmp = loadPixelArrayRgb(nx);
        const auto pzBmp = loadPixelArrayRgb(pz);
        const auto nzBmp = loadPixelArrayRgb(nz);

        return make_shared<CubeMap<RGB>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    shared_ptr<CubeMap<RGBA>> loadCubemapRgba(
            const string &px,
            const string &nx,
            const string &py,
            const string &ny,
            const string &pz,
            const string &nz
    ) {
        const auto pxBmp = loadPixelArrayRgba(px);
        const auto nxBmp = loadPixelArrayRgba(nx);
        const auto pyBmp = loadPixelArrayRgba(py);
        const auto nyBmp = loadPixelArrayRgba(nx);
        const auto pzBmp = loadPixelArrayRgba(pz);
        const auto nzBmp = loadPixelArrayRgba(nz);

        return make_shared<CubeMap<RGBA>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    shared_ptr<CubeMap<RGBFloat>> loadCubemapRgbFloat(
            const string &px,
            const string &nx,
            const string &py,
            const string &ny,
            const string &pz,
            const string &nz
    ) {
        const auto pxBmp = loadPixelArrayRgbFloat(px);
        const auto nxBmp = loadPixelArrayRgbFloat(nx);
        const auto pyBmp = loadPixelArrayRgbFloat(py);
        const auto nyBmp = loadPixelArrayRgbFloat(nx);
        const auto pzBmp = loadPixelArrayRgbFloat(pz);
        const auto nzBmp = loadPixelArrayRgbFloat(nz);

        return make_shared<CubeMap<RGBFloat>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    shared_ptr<CubeMap<RGBAFloat>> loadCubemapRgbaFloat(
            const string &px,
            const string &nx,
            const string &py,
            const string &ny,
            const string &pz,
            const string &nz
    ) {
        const auto pxBmp = loadPixelArrayRgbaFloat(px);
        const auto nxBmp = loadPixelArrayRgbaFloat(nx);
        const auto pyBmp = loadPixelArrayRgbaFloat(py);
        const auto nyBmp = loadPixelArrayRgbaFloat(nx);
        const auto pzBmp = loadPixelArrayRgbaFloat(pz);
        const auto nzBmp = loadPixelArrayRgbaFloat(nz);

        return make_shared<CubeMap<RGBAFloat>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    template<class T, int N>
    void write(const std::string &path, const ShCoefficients<T, N> &item) {
        //@todo:
    }
}
#endif //SH_UTILS_H
