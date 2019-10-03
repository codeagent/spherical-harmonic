//
// Created by Alex on 12.09.2019.
//

#ifndef SH_UTILS_H
#define SH_UTILS_H

#include <memory>
#include <inttypes.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <type_traits>


#include <stb_image.h>
#include <stb_image_write.h>
#include <json.h>

#include "PixelArray.h"
#include "pixel_format.h"
#include "CubeMap.h"
#include "spherical_harmonic.h"

namespace sh {
    using namespace std;

    enum FileFormat {
        Png,
        Bmp,
        Tga,
        Jpg,
        Hdr
    };

    shared_ptr<PixelArray<RGBF>> loadPixelArrayRgb(const string &path) {
        stbi_set_flip_vertically_on_load(1);
        int width, height, channels;
        float *data = stbi_loadf(path.c_str(), &width, &height, &channels, 3);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGBF>>((RGBF *) data, width, height);
    }

    shared_ptr<PixelArray<RGBAF>> loadPixelArrayRgba(const string &path) {
        stbi_set_flip_vertically_on_load(1);
        int width, height, channels;
        float *data = stbi_loadf(path.c_str(), &width, &height, &channels, 4);
        if (!data) {
            throw std::runtime_error("Failed to load image '" + path + "' due to reason: " + stbi_failure_reason());
        }
        return make_shared<PixelArray<RGBAF>>((RGBAF *) data, width, height);
    }


    shared_ptr<CubeMap<RGBF>> loadCubemapRgb(
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
        const auto nyBmp = loadPixelArrayRgb(ny);
        const auto pzBmp = loadPixelArrayRgb(pz);
        const auto nzBmp = loadPixelArrayRgb(nz);

        return make_shared<CubeMap<RGBF>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    shared_ptr<CubeMap<RGBAF>> loadCubemapRgba(
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
        const auto nyBmp = loadPixelArrayRgba(ny);
        const auto pzBmp = loadPixelArrayRgba(pz);
        const auto nzBmp = loadPixelArrayRgba(nz);

        return make_shared<CubeMap<RGBAF>>(pxBmp, nxBmp, pyBmp, nyBmp, pzBmp, nzBmp);
    }

    std::ostream &operator<<(std::ostream &stream, const ShCoefficients<RGB> &h) {

        stream << "{" << std::endl;
        stream << "\t\"order\": " << order(h) << ", " << std::endl;
        stream << "\t\"channels\": {" << std::endl;
        stream << "\t\t\"red\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->r;
        }
        stream << "]," << std::endl;
        stream << "\t\t\"green\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->g;
        }
        stream << "]," << std::endl;
        stream << "\t\t\"blue\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->b;
        }
        stream << "]" << std::endl;
        stream << "\t}" << std::endl;
        stream << "}";
        return stream;
    }

    std::ostream &operator<<(std::ostream &stream, const ShCoefficients<RGBA> &h) {

        stream << "{" << std::endl;
        stream << "\t\"order\": " << order(h) << ", " << std::endl;
        stream << "\t\"channels\": {" << std::endl;
        stream << "\t\t\"red\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->r;
        }
        stream << "]," << std::endl;
        stream << "\t\t\"green\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->g;
        }
        stream << "]," << std::endl;
        stream << "\t\t\"blue\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->b;
        }
        stream << "]," << std::endl;
        stream << "\t\t\"alpha\": [";
        for (auto it = h.begin(); it != h.end(); it++) {
            if (it != h.begin()) {
                stream << ", ";
            }
            stream << it->a;
        }
        stream << "]" << std::endl;
        stream << "\t}" << std::endl;
        stream << "}";
        return stream;
    }

    template<class T>
    void write(const std::string &path, const ShCoefficients<T> &item) {
        std::ofstream f;
        f.open(path);
        f << item;
        f.close();
    }

    inline stbi_uc *hdr2ldr(const PixelArray<RGBF> &bitmap) {
        const auto w = bitmap.getWidth(), h = bitmap.getHeight();
        const auto bytes = sizeof(RGBF) * w * h;
        auto *data = (float *) STBI_MALLOC(bytes);
        memcpy(data, bitmap.getData(), bytes);
        return stbi__hdr_to_ldr(data, w, h, 3);
    }

    inline stbi_uc *hdr2ldr(const PixelArray<RGBAF> &bitmap) {
        const auto w = bitmap.getWidth(), h = bitmap.getHeight();
        const auto bytes = sizeof(RGBAF) * w * h;
        auto *data = (float *) STBI_MALLOC(bytes);
        memcpy(data, bitmap.getData(), bytes);
        return stbi__hdr_to_ldr(data, w, h, 4);
    }

    void write(const std::string &path, const FileFormat format, const std::shared_ptr<CubeMap<RGBF>> &cubemap,
            const std::string &prefix = "") {
        using namespace std;

        stbi_flip_vertically_on_write(1);
        map<CubeMapFaceEnum, string> faceToNameLookup = {
                {CubeMapFaceEnum::PositiveX, "posx"s},
                {CubeMapFaceEnum::NegativeX, "negx"s},
                {CubeMapFaceEnum::PositiveY, "posy"s},
                {CubeMapFaceEnum::NegativeY, "negy"s},
                {CubeMapFaceEnum::PositiveZ, "posz"s},
                {CubeMapFaceEnum::NegativeZ, "negz"s}
        };

        const auto w = cubemap->getWidth(), h = cubemap->getHeight();
        for (auto &item: faceToNameLookup) {
            const CubeMapFaceEnum face = item.first;
            const string name = item.second;
            const auto bitmap = (*cubemap)[face];

            if (format == FileFormat::Png) {
                const auto filename = path + "/"s + prefix + name + ".png"s;
                if (!stbi_write_png(filename.c_str(), w, h, 3, hdr2ldr(*bitmap), 0)) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Bmp) {
                const auto filename = path + "/"s + prefix + name + ".bmp"s;
                if (!stbi_write_bmp(filename.c_str(), w, h, 3, hdr2ldr(*bitmap))) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Tga) {
                const auto filename = path + "/"s + prefix + name + ".tga"s;
                if (!stbi_write_tga(filename.c_str(), w, h, 3, hdr2ldr(*bitmap))) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Jpg) {
                const auto filename = path + "/"s + prefix + name + ".jpg"s;
                if (!stbi_write_jpg(filename.c_str(), w, h, 3, hdr2ldr(*bitmap), 95)) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Hdr) {
                const auto filename = path + "/"s + prefix + name + ".hdr"s;
                if (!stbi_write_hdr(filename.c_str(), w, h, 3, (const float *) bitmap->getData())) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }
        }
    };

    void write(const std::string &path, const FileFormat format, const std::shared_ptr<CubeMap<RGBAF>> &cubemap,
            const std::string &prefix = "") {
        using namespace std;
        stbi_flip_vertically_on_write(1);
        map<CubeMapFaceEnum, string> faceToNameLookup = {
                {CubeMapFaceEnum::PositiveX, "posx"s},
                {CubeMapFaceEnum::NegativeX, "negx"s},
                {CubeMapFaceEnum::PositiveY, "posy"s},
                {CubeMapFaceEnum::NegativeY, "negy"s},
                {CubeMapFaceEnum::PositiveZ, "posz"s},
                {CubeMapFaceEnum::NegativeZ, "negz"s}
        };

        const auto w = cubemap->getWidth(), h = cubemap->getHeight();
        for (auto &item: faceToNameLookup) {
            const CubeMapFaceEnum face = item.first;
            const string name = item.second;
            const auto bitmap = (*cubemap)[face];

            if (format == FileFormat::Png) {
                const auto filename = path + "/"s + prefix + name + ".png"s;
                if (!stbi_write_png(filename.c_str(), w, h, 4, hdr2ldr(*bitmap), 0)) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Bmp) {
                const auto filename = path + "/"s + prefix + name + ".bmp"s;
                if (!stbi_write_bmp(filename.c_str(), w, h, 4, hdr2ldr(*bitmap))) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Tga) {
                const auto filename = path + "/"s + prefix + name + ".tga"s;
                if (!stbi_write_tga(filename.c_str(), w, h, 4, hdr2ldr(*bitmap))) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Jpg) {
                const auto filename = path + "/"s + prefix + name + ".jpg"s;
                if (!stbi_write_jpg(filename.c_str(), w, h, 4, hdr2ldr(*bitmap), 95)) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }

            if (format == FileFormat::Hdr) {
                const auto filename = path + "/"s + prefix + name + ".hdr"s;
                if (!stbi_write_hdr(filename.c_str(), w, h, 4, (const float *) bitmap->getData())) {
                    throw runtime_error("Failed to write to file: '" + filename + "'");
                }
            }
        }
    };


    static map<string, vector<float>> _read(const string &path) {
        using namespace std;
        ifstream f(path);

        if (!f) {
            throw runtime_error("Failed to open file: '" + path + "'");
        }
        string str((std::istreambuf_iterator<char>(f)),
                std::istreambuf_iterator<char>());


        json_value_s *root = json_parse(str.c_str(), str.length());
        auto *object = (json_object_s *) root->payload;

        map<string, vector<float>> channelsMap;
        for (auto *element = object->start; element; element = element->next) {
            const string name = element->name->string;
            json_value_s *value = element->value;

            if (name == "channels"s) {
                auto *channels = (json_object_s *) value->payload;
                for (json_object_element_s *channel = channels->start; channel; channel = channel->next) {
                    const string channelName = channel->name->string;
                    json_value_s *value = channel->value;

                    auto *coeffArray = (json_array_s *) value->payload;
                    for (auto *coeff = coeffArray->start; coeff; coeff = coeff->next) {
                        auto *coeffValue = (json_number_s *) coeff->value->payload;
                        const string coeffStr = coeffValue->number;
                        float coeffVal = stof(coeffStr);
                        channelsMap[channelName].push_back(coeffVal);
                    }
                }
            }
        }
        free(root);
        return channelsMap;
    }

    ShCoefficients<RGB> readRgb(const std::string &path) {
        using namespace std;

        auto channels = _read(path);
        ShCoefficients<RGB> coefficients;

        if (channels.empty()) {
            return coefficients;
        }

        const auto size = channels.begin()->second.size();
        coefficients.resize(size);

        for (auto pair: channels) {
            const auto channel = pair.first;
            const auto values = pair.second;
            auto i = 0;
            for (auto &value: values) {
                if (channel == "red"s) {
                    coefficients[i].r = value;
                } else if (channel == "green"s) {
                    coefficients[i].g = value;
                } else if (channel == "blue"s) {
                    coefficients[i].b = value;
                }
                i++;
            }
        }

        return coefficients;
    }

    ShCoefficients<RGBA> readRgba(const std::string &path) {
        using namespace std;

        auto channels = _read(path);
        ShCoefficients<RGBA> coefficients;

        if (channels.empty()) {
            return coefficients;
        }

        const auto size = channels.begin()->second.size();
        coefficients.resize(size);

        for (auto pair: channels) {
            const auto channel = pair.first;
            const auto values = pair.second;
            auto i = 0;
            for (auto &value: values) {
                if (channel == "red"s) {
                    coefficients[i].r = value;
                } else if (channel == "green"s) {
                    coefficients[i].g = value;
                } else if (channel == "blue"s) {
                    coefficients[i].b = value;
                } else if (channel == "alpha"s) {
                    coefficients[i].a = value;
                }
                i++;
            }
        }
        return coefficients;
    }
}
#endif //SH_UTILS_H
