//
// Created by Alex on 10.09.2019.
//

#ifndef SH_CUBEMAP_H
#define SH_CUBEMAP_H

#include <map>
#include <memory>

#include "PixelArray.h"
#include "math.h"

namespace SH {

    enum CubeMapFaceEnum {
        PositiveX,
        NegativeX,
        PositiveY,
        NegativeY,
        PositiveZ,
        NegativeZ,
    };

    template<class T>
    class CubeMap {
    public:
        using CubeMapFace = std::shared_ptr<PixelArray<T>>;
        using CubeMapFaceCollection = std::map<CubeMapFaceEnum, CubeMapFace>;
    protected:
        CubeMapFaceCollection faces;
    public:
        CubeMap(
                CubeMapFace px,
                CubeMapFace nx,
                CubeMapFace py,
                CubeMapFace ny,
                CubeMapFace pz,
                CubeMapFace nz
        ) {
            faces[CubeMapFaceEnum::PositiveX] = px;
            faces[CubeMapFaceEnum::NegativeX] = nx;
            faces[CubeMapFaceEnum::PositiveY] = py;
            faces[CubeMapFaceEnum::NegativeY] = ny;
            faces[CubeMapFaceEnum::PositiveZ] = pz;
            faces[CubeMapFaceEnum::NegativeZ] = nz;
        }

        CubeMap(const CubeMap &) = delete;
        CubeMap &operator=(const CubeMap &) = delete;

        CubeMapFace operator[](CubeMapFaceEnum face) {
            return faces[face];
        }

        uint16_t getWidth() const {
            return faces.begin()->first->getWidth();
        }

        uint16_t getHeight() const {
            return faces.begin()->first->getHeight();
        }
    };
}
#endif //SH_CUBEMAP_H
