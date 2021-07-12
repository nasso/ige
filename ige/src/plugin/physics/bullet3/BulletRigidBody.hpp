#ifndef F564463C_F6C2_4B7F_86AB_387B624C888B
#define F564463C_F6C2_4B7F_86AB_387B624C888B

#include "igepch.hpp"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/physics/Collider.hpp"
#include "ige/plugin/physics/RigidBody.hpp"

namespace ige::bt {

class BulletRigidBody {
public:
    BulletRigidBody(
        const plugin::physics::RigidBody& rigidbody,
        const plugin::transform::Transform& transform,
        std::shared_ptr<btDynamicsWorld> world);

    BulletRigidBody(BulletRigidBody&&);

    ~BulletRigidBody();

    BulletRigidBody& operator=(BulletRigidBody&&);

    btRigidBody* body();

    void update(
        const plugin::transform::Transform& transform,
        plugin::physics::RigidBody& rigidbody);

    void sync_ige_entity(ecs::World& wld, ecs::EntityId entity);

    bool operator==(const BulletRigidBody& rhs);
    bool operator==(const btRigidBody* rhs);
    bool operator==(const btCollisionObject* rhs);

private:
    void set_rigibody_shape(const plugin::physics::Collider& collider);

    std::unique_ptr<btRigidBody> m_rigidbody;
    std::unique_ptr<btDefaultMotionState> m_motion_state;
    std::unique_ptr<btCollisionShape> m_colShape;
    std::weak_ptr<btDynamicsWorld> m_world;
    bool m_moved = false;
};

}

#endif /* F564463C_F6C2_4B7F_86AB_387B624C888B */
