//
// Created by Alex on 16.09.2019.
//

#ifndef SH_CUBEMAPPOLARFUNCTION_H
#define SH_CUBEMAPPOLARFUNCTION_H

#include <memory>

#include "math.h"
#include "sampling.h"
#include "CubeMap.h"

namespace sh {
    template<class T>
    class CubeMapPolarFunction {
    protected:
        std::shared_ptr<CubeMap<T>> cubemap;
        InterpolationMethod filtering;
    public:
        CubeMapPolarFunction(std::shared_ptr<CubeMap<T>> cubemap, InterpolationMethod filtering) :
                cubemap(cubemap), filtering(filtering) {}

        T operator()(float phi, float tetta) {
            return sampleCubemap<T>(*cubemap, math::sphericalToCartesian(phi, tetta), filtering);
        }
    };
}
#endif //SH_CUBEMAPPOLARFUNCTION_H
