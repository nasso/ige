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
        const plugin::physic::RigidBody& rigidbody,
        const plugin::transform::Transform& transform, btDynamicsWorld* world);

    btRigidBody* body();

    void update(ecs::World& wld, ecs::EntityId entity);

    void sync_ige_entity(ecs::World& wld, ecs::EntityId entity);

    bool operator==(const BulletRigidBody& rhs);
    bool operator==(const btRigidBody* rhs);

private:
    void set_rigibody_shape(const plugin::physic::Collider& collider);
    void update(
        const plugin::transform::Transform& transform,
        plugin::physic::RigidBody& rigidbody);

    std::unique_ptr<btRigidBody> m_rigidbody;
    std::unique_ptr<btDefaultMotionState> m_motion_state;
    std::unique_ptr<btCollisionShape> m_colShape;
};
}

#endif /* F564463C_F6C2_4B7F_86AB_387B624C888B */
