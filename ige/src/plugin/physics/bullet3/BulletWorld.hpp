#ifndef A17BC820_E47A_445F_A85E_1D87FE8FB1EA
#define A17BC820_E47A_445F_A85E_1D87FE8FB1EA

#include "igepch.hpp"

#include "btBulletDynamicsCommon.h"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/physics/GhostObject.hpp"
#include "ige/plugin/physics/PhysicsWorld.hpp"
#include "ige/plugin/physics/RigidBody.hpp"

namespace ige::bt {

class BulletWorld {
public:
    BulletWorld(btVector3 gravity = { 0, -10, 0 });

    void clean_world();
    void new_rigidbody(
        ecs::World& wld, const ecs::EntityId& entity,
        const plugin::physics::RigidBody& rigidbody,
        const plugin::transform::Transform& transform);
    void new_ghost_object(
        ecs::World& wld, const ecs::EntityId& entity,
        const plugin::physics::GhostObject& object,
        const plugin::transform::Transform& transform);

    void simulate(float time_step);

    void get_collisions(ecs::World& wld, plugin::physics::PhysicsWorld& world);

    static void tick_update(btDynamicsWorld* world, btScalar timeStep);

    void new_constraint(
        ecs::World& wld, const plugin::physics::Constraint& constraint);

private:
    btDefaultCollisionConfiguration m_collision_config;
    btCollisionDispatcher m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    btSequentialImpulseConstraintSolver m_solver;
    std::shared_ptr<btDiscreteDynamicsWorld> m_world;
    std::vector<std::pair<const btCollisionObject*, const btCollisionObject*>>
        m_collisions;
    std::unordered_set<std::unique_ptr<btTypedConstraint>> m_constraints;
};

}

#endif /* A17BC820_E47A_445F_A85E_1D87FE8FB1EA */
