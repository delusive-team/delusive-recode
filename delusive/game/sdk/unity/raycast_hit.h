#pragma once
#include "../math/vector.h"

namespace unity {
    struct raycast_hit_t {
        vec3_t m_Point;
        vec3_t m_Normal;
        uint32_t m_FaceID;
        float m_Distance;
        vec2_t m_UV;
        int m_Collider;
    };
}
