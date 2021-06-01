#ifndef A17BC820_E47A_445F_A85E_1D87FE8FB1EA
#define A17BC820_E47A_445F_A85E_1D87FE8FB1EA

#include "btBulletDynamicsCommon.h"
#include "ige/ecs.hpp"
#include "ige/plugin/physic/PhysicWorld.hpp"
#include <memory>
#include <set>
#include <utility>
#include <vector>

namespace ige::bt {
class BulletWorld {
public:
    BulletWorld(btVector3 gravity = { 0, -10, 0 });

    void clean_world();
    void new_entity(ecs::World& wld, ecs::EntityId entity);

    void simulate(float time_step);

    void get_collisions(ecs::World& wld, plugin::physic::PhysicWorld& world);

    static void tick_update(btDynamicsWorld* world, btScalar timeStep);

    void new_constraint(
        ecs::World& wld, const plugin::physic::Constraint& constraint);

private:
    btDefaultCollisionConfiguration m_collision_config;
    btCollisionDispatcher m_dispatcher;
    std::unique_ptr<btBroadphaseInterface> m_broadphase;
    btSequentialImpulseConstraintSolver m_solver;
    btDiscreteDynamicsWorld m_world;
    std::vector<std::pair<const btRigidBody*, const btRigidBody*>> m_collisions;
    std::set<std::unique_ptr<btTypedConstraint>> m_constraints;
};
}

#endif /* A17BC820_E47A_445F_A85E_1D87FE8FB1EA */
