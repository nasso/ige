#include "BulletWorld.hpp"
#include "BulletRigidBody.hpp"
#include "ige/ecs.hpp"
#include "ige/plugin.hpp"
#include <optional>

using ige::bt::BulletWorld;
using ige::ecs::EntityId;
using ige::ecs::World;
using ige::plugin::physics::Constraint;
using ige::plugin::physics::PhysicsWorld;
using ige::plugin::physics::RigidBody;
using ige::plugin::transform::Transform;

BulletWorld::BulletWorld(btVector3 gravity)
    : m_dispatcher(&m_collision_config)
    , m_broadphase(new btDbvtBroadphase)
    , m_world(&m_dispatcher, m_broadphase.get(), &m_solver, &m_collision_config)
{
    m_world.setGravity(gravity);
    m_world.setInternalTickCallback(tick_update, this);
}

void BulletWorld::clean_world()
{
    for (int i = m_world.getNumConstraints() - 1; i >= 0; i--) {
        m_world.removeConstraint(m_world.getConstraint(i));
    }
    for (int i = m_world.getNumCollisionObjects() - 1; i >= 0; i--) {
        btCollisionObject* obj = m_world.getCollisionObjectArray()[i];
        m_world.removeCollisionObject(obj);
    }
}

void BulletWorld::new_entity(World& wld, EntityId entity)
{
    auto transform = wld.get_component<Transform>(entity);
    auto rigidbody = wld.get_component<RigidBody>(entity);

    wld.emplace_component<BulletRigidBody>(
        entity, *rigidbody, *transform, &m_world);
}

void BulletWorld::new_constraint(World& wld, const Constraint& constraint)
{
    auto rigidbody = wld.get_component<BulletRigidBody>(constraint.entity);

    if (!rigidbody)
        return;
    auto constraint_ptr = static_cast<btGeneric6DofConstraint*>(
        m_constraints
            .insert(std::make_unique<btGeneric6DofConstraint>(
                *rigidbody->body(), btTransform::getIdentity(), false))
            .first->get());
    m_world.addConstraint(constraint_ptr);
    constraint_ptr->setDbgDrawSize(5.0f);

    constraint_ptr->setAngularLowerLimit({
        constraint.angular_lower_limit.x,
        constraint.angular_lower_limit.y,
        constraint.angular_lower_limit.z,
    });
    constraint_ptr->setAngularUpperLimit({
        constraint.angular_upper_limit.x,
        constraint.angular_upper_limit.y,
        constraint.angular_upper_limit.z,
    });
    constraint_ptr->setLinearLowerLimit({
        constraint.linear_lower_limit.x,
        constraint.linear_lower_limit.y,
        constraint.linear_lower_limit.z,
    });
    constraint_ptr->setLinearUpperLimit({
        constraint.linear_upper_limit.x,
        constraint.linear_upper_limit.y,
        constraint.linear_upper_limit.z,
    });
}

void BulletWorld::simulate(float time_step)
{
    m_world.stepSimulation(time_step);
}

void BulletWorld::get_collisions(World& wld, PhysicsWorld& phys_world)
{
    auto rigidbodies = wld.query<BulletRigidBody>();

    for (auto [fst_body, snd_body] : m_collisions) {
        std::optional<EntityId> entity1;
        std::optional<EntityId> entity2;

        for (const auto& [entity, body] : rigidbodies) {
            if (body == fst_body) {
                entity1 = entity;
            }

            if (body == snd_body) {
                entity2 = entity;
            }

            if (entity1 && entity2) {
                break;
            }
        }

        if (entity1 || entity2) {
            phys_world.add_collision(entity1.value(), entity2.value());
        }
    }
}

void BulletWorld::tick_update(btDynamicsWorld* dynamicsWorld, btScalar)
{
    auto self = static_cast<BulletWorld*>(dynamicsWorld->getWorldUserInfo());

    self->m_collisions.clear();
    auto& dispatcher = *dynamicsWorld->getDispatcher();
    const int num_manifolds = dispatcher.getNumManifolds();
    for (int manifold_idx = 0; manifold_idx < num_manifolds; ++manifold_idx) {
        auto& manifold = *dispatcher.getManifoldByIndexInternal(manifold_idx);

        self->m_collisions.push_back({
            static_cast<const btRigidBody*>(manifold.getBody0()),
            static_cast<const btRigidBody*>(manifold.getBody1()),
        });
    }
}
