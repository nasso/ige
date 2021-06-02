#ifndef F564463C_F6C2_4B7F_86AB_387B624C888B
#define F564463C_F6C2_4B7F_86AB_387B624C888B

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "ige.hpp"
#include <memory>

namespace ige::bt {

class BulletRigidBody {
public:
    BulletRigidBody(
        const plugin::physics::RigidBody& rigidbody,
        const plugin::transform::Transform& transform, btDynamicsWorld* world);

    btRigidBody* body();

    void update(
        const plugin::transform::Transform& transform,
        plugin::physics::RigidBody& rigidbody);

    void sync_ige_entity(ecs::World& wld, ecs::EntityId entity);

    bool operator==(const BulletRigidBody& rhs);
    bool operator==(const btRigidBody* rhs);

private:
    void set_rigibody_shape(const plugin::physics::Collider& collider);

    std::unique_ptr<btRigidBody> m_rigidbody;
    std::unique_ptr<btDefaultMotionState> m_motion_state;
    std::unique_ptr<btCollisionShape> m_colShape;
};

}

#endif /* F564463C_F6C2_4B7F_86AB_387B624C888B */
