#pragma once
#include "../math/vector.h"

namespace unity {
    struct ray_t {
        vec3_t origin;
        vec3_t dir;

        ray_t()
        {
        }

        ray_t(vec3_t o, vec3_t d)
        {
            origin = o;
            dir = d;
        }
    };
}
