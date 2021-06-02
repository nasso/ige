#ifndef DEF0501C_911A_4C81_B5D5_8E8270F03E94
#define DEF0501C_911A_4C81_B5D5_8E8270F03E94

#include <glm/vec3.hpp>

namespace ige::plugin::physics {

enum class ColliderType { SPHERE, BOX, CAPSULE };

struct SphereCollider {
    float radius;
};

struct CapsuleCollider {
    float height;
    float radius;
};

struct BoxCollider {
    glm::vec3 extents;
};

struct Collider {
    ColliderType type;
    union {
        SphereCollider sphere;
        BoxCollider box;
        CapsuleCollider capsule;
    };
};

}

#endif /* DEF0501C_911A_4C81_B5D5_8E8270F03E94 */
