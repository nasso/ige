#include "ige/plugin/PhysicPlugin.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/physic/RigidBody.hpp"
#include "physic/bullet3/BulletRigidBody.hpp"
#include "physic/bullet3/BulletWorld.hpp"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include <algorithm>
#include <chrono>

using ige::bt::BulletRigidBody;
using ige::bt::BulletWorld;
using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::physic::PhysicPlugin;
using ige::plugin::physic::PhysicWorld;
using ige::plugin::physic::RigidBody;
using ige::plugin::time::Time;
using std::chrono::duration;
using std::chrono::duration_cast;

static void setup_physic_system(World& wld)
{
    wld.emplace<BulletWorld>();
    wld.emplace<PhysicWorld>();
}

static void clean_physic_system(World& wld)
{
    auto bt_world = wld.get<BulletWorld>();

    if (bt_world) {
        bt_world->clean_world();
    }
}

static void physic_entities_update(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    auto bullet_world = wld.get<BulletWorld>();
    for (auto [entity, body] : wld.query<RigidBody>()) {
        auto rigidbody = wld.get_component<BulletRigidBody>(entity);

        if (rigidbody) {
            rigidbody->update(wld, entity);
        } else {
            bullet_world->new_entity(wld, entity);
        }
    }
}

static void ige_entities_update(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    for (auto [entity, body] : wld.query<BulletRigidBody>()) {
        body.sync_ige_entity(wld, entity);
    }
}

static void physic_world_simulate(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    if (auto time = wld.get<Time>()) {
        if (time->ticks() == 0) {
            return;
        }
        for (std::uint32_t i = 0; i < time->ticks(); i++) {
            dynamicsWorld->simulate(
                duration_cast<duration<float>>(time->tick()).count());
        }
        ige_entities_update(wld);
    }
}

void collisions_update(World& wld)
{
    auto bt_world = wld.get<BulletWorld>();
    auto physic_world = wld.get<PhysicWorld>();

    if (!bt_world || !physic_world) {
        return;
    }
    physic_world->clear_collisions();
    bt_world->get_collisions(wld, *physic_world);
}

void constraints_update(World& wld)
{
    auto bt_world = wld.get<BulletWorld>();
    auto physic_world = wld.get<PhysicWorld>();

    if (!bt_world || !physic_world) {
        return;
    }
    auto constraints = physic_world->get_new_constraints();

    std::for_each(
        constraints.begin(), constraints.end(),
        [&](auto& constraint) { bt_world->new_constraint(wld, constraint); });
    physic_world->clear_new_constraints();
}

void PhysicPlugin::plug(App::Builder& builder) const
{
    builder.add_startup_system(System(setup_physic_system));
    builder.add_system(System(physic_entities_update));
    builder.add_system(System(physic_world_simulate));
    builder.add_system(System(collisions_update));
    builder.add_system(System(constraints_update));
    builder.add_cleanup_system(System(clean_physic_system));
}
