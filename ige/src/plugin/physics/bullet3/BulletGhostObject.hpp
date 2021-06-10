#ifndef AEBE5777_3041_432D_8A6B_F977321EC36E
#define AEBE5777_3041_432D_8A6B_F977321EC36E

#include "BulletCollision/CollisionDispatch/btGhostObject.h"
#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "ige.hpp"
#include <memory>

namespace ige::bt {
class BulletGhostObject {
public:
    BulletGhostObject(
        const plugin::physics::GhostObject& object,
        const plugin::transform::Transform& transform,
        std::shared_ptr<btDynamicsWorld> world);

    BulletGhostObject(BulletGhostObject&&);

    ~BulletGhostObject();

    BulletGhostObject& operator=(BulletGhostObject&&);

    void update(
        const plugin::transform::Transform& transform,
        plugin::physics::GhostObject& object);

    bool operator==(const btCollisionObject* rhs);

private:
    void set_object_shape(const plugin::physics::Collider& collider);

    std::unique_ptr<btGhostObject> m_object;
    std::unique_ptr<btCollisionShape> m_colShape;
    std::weak_ptr<btDynamicsWorld> m_world;
    bool m_moved = false;
};
}

#endif /* AEBE5777_3041_432D_8A6B_F977321EC36E */
