//
// Created by Alex on 01.10.2019.
//

#ifndef SPHERICALHARMONIC_REAL_H
#define SPHERICALHARMONIC_REAL_H

#include <glm/glm.hpp>

namespace sh {
#if defined(FLOAT_DOUBLE)
    using real = double;
    using mat3 = glm::dmat3;
    using vec3 = glm::dvec3;
    using vec2 = glm::dvec2;
#else
    using real = float;
    using mat3 = glm::mat3;
    using vec3 = glm::vec3;
    using vec2 = glm::vec2;
#endif
}
#endif //SPHERICALHARMONIC_REAL_H
