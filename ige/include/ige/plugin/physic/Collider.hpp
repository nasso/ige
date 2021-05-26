#ifndef DEF0501C_911A_4C81_B5D5_8E8270F03E94
#define DEF0501C_911A_4C81_B5D5_8E8270F03E94

namespace ige {
namespace plugin {
    namespace physic {

        enum class ColliderType { SPHERE, SQUARE, CYLINDER };

        struct SphereCollider {
            float radius;
        };

        struct SquareCollider {
            int height;
            int width;
        };

        struct CylinderCollider {
            int height;
            float radius;
        };

        struct Collider {
            ColliderType type;
            union {
                SphereCollider sphere;
                SquareCollider square;
                CylinderCollider cylinder;
            };
        };
    }
}
}

#endif /* DEF0501C_911A_4C81_B5D5_8E8270F03E94 */
