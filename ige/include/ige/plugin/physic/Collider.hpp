#ifndef DEF0501C_911A_4C81_B5D5_8E8270F03E94
#define DEF0501C_911A_4C81_B5D5_8E8270F03E94

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige::plugin::physic {

enum class ColliderType { SPHERE, BOX, CAPSULE };

struct SphereCollider {
    float radius;
};

struct CapsuleCollider {
    int height;
    float radius;
};

struct Collider {
    ColliderType type;
    union {
        SphereCollider sphere;
        glm::vec3 box;
        CapsuleCollider capsule;
    };
};
}

#endif /* DEF0501C_911A_4C81_B5D5_8E8270F03E94 */
