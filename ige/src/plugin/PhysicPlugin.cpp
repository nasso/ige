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
#include <iostream>

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

static void physic_entities_update(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    auto rigidbody_storage = wld.get_component_storage<RigidBody>();

    if (rigidbody_storage) {
        std::for_each(
            rigidbody_storage->begin(), rigidbody_storage->end(),
            [&](auto& pair) {
                auto rigidbody = wld.get_component<BulletRigidBody>(pair.first);

                if (!rigidbody) {
                    auto bullet_world = wld.get<BulletWorld>();

                    bullet_world->new_entity(wld, pair.first);
                }
                rigidbody->update(wld, pair.first);
            });
    }
}

static void physic_world_simulate(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    if (auto time = wld.get<Time>()) {
        for (std::uint32_t i = 0; i < time->ticks(); i++) {
            dynamicsWorld->simulate(
                duration_cast<duration<float>>(time->tick()).count());
        }
    }
}

static void ige_entities_update(World& wld)
{
    auto dynamicsWorld = wld.get<BulletWorld>();

    if (!dynamicsWorld) {
        return;
    }
    auto rigidbody_storage = wld.get_component_storage<BulletRigidBody>();

    if (rigidbody_storage) {
        std::for_each(
            rigidbody_storage->begin(), rigidbody_storage->end(),
            [&](auto& pair) { pair.second.sync_ige_entity(wld, pair.first); });
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

void PhysicPlugin::plug(App::Builder& builder) const
{
    builder.add_startup_system(System(setup_physic_system));
    // builder.add_system(System(physic_world_update));
    builder.add_system(System(physic_entities_update));
    builder.add_system(System(physic_world_simulate));
    builder.add_system(System(ige_entities_update));
    builder.add_system(System(collisions_update));
}
