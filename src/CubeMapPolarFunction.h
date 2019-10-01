//
// Created by Alex on 16.09.2019.
//

#ifndef SH_CUBEMAPPOLARFUNCTION_H
#define SH_CUBEMAPPOLARFUNCTION_H

#include <memory>

#include "real.h"
#include "math.h"
#include "sampling.h"
#include "CubeMap.h"

namespace sh {
    template<class R, class F>
    class CubeMapPolarFunction {
    protected:
        std::shared_ptr<CubeMap<F>> cubemap;
        InterpolationMethod filtering;
    public:
        CubeMapPolarFunction(std::shared_ptr<CubeMap<F>> cubemap, InterpolationMethod filtering) :
                cubemap(cubemap), filtering(filtering) {}

        R operator()(real phi, real tetta) {
            return R(sampleCubemap<F>(*cubemap, math::sphericalToCartesian(phi, tetta), filtering));
        }
    };
}
#endif //SH_CUBEMAPPOLARFUNCTION_H
