#include "ige/plugin/PhysicPlugin.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/physic/RigidBody.hpp"

#include "btBulletCollisionCommon.h"
#include "btBulletDynamicsCommon.h"

#include <algorithm>
#include <iostream>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::physic::PhysicPlugin;
using ige::plugin::physic::PhysicWorld;
using ige::plugin::physic::RigidBody;
using ige::plugin::physic::RigidBodyStatus;
using ige::plugin::transform::Transform;

static void setup_physic_system(World& wld)
{
    wld.emplace<PhysicWorld>();
}

static void update_bullet_world_entity(
    World& wld, btDiscreteDynamicsWorld* dynamicsWorld, EntityId entity,
    RigidBody& rigidbody)
{
    auto transform = wld.get_component<Transform>(entity);

    if (rigidbody.status() == RigidBodyStatus::NEW) {
        vec3 position = transform->world_translation();
        quat rotation = transform->rotation();
        btTransform bt_transform;

        bt_transform.setRotation({
            rotation.x,
            rotation.y,
            rotation.z,
            rotation.w,
        });
        bt_transform.setOrigin(btVector3(position.x, position.y, position.z));
        btDefaultMotionState* myMotionState
            = new btDefaultMotionState(bt_transform);

        btCollisionShape* colShape
            = new btBoxShape(btVector3(0.5f, 0.5f, 0.5f));

        btRigidBody::btRigidBodyConstructionInfo rbInfo(
            rigidbody.get_mass(), myMotionState, colShape);
        rbInfo.m_restitution = 0.75f;
        btRigidBody* bt_rigidbody = new btRigidBody(rbInfo);
        if (rigidbody.is_kinematic()) {
            bt_rigidbody->setCollisionFlags(
                bt_rigidbody->getCollisionFlags()
                | btCollisionObject::CF_NO_CONTACT_RESPONSE);
            bt_rigidbody->setActivationState(DISABLE_DEACTIVATION);
        }
        dynamicsWorld->addRigidBody(bt_rigidbody);
        rigidbody.initialize();
        wld.add_component(entity, bt_rigidbody);
        return;
    } else if (rigidbody.status() == RigidBodyStatus::DIRTY) {
        auto bt_rigidbody = wld.get_component<btRigidBody*>(entity);

        vec3 position = transform->world_translation();
        quat rotation = transform->rotation();
        btTransform bt_transform;

        bt_transform.setRotation({
            rotation.x,
            rotation.y,
            rotation.z,
            rotation.w,
        });
        bt_transform.setOrigin(btVector3(position.x, position.y, position.z));
        (*bt_rigidbody)->getMotionState()->setWorldTransform(bt_transform);
        (*bt_rigidbody)
            ->applyCentralImpulse(btVector3(0, rigidbody.apply_force(), 0));
    }
}

static void update_ige_entity(
    World& wld, btDiscreteDynamicsWorld* dynamicsWorld, EntityId entity,
    RigidBody& rigidbody)
{
    auto transform = wld.get_component<Transform>(entity);

    auto bt_rigidbody = wld.get_component<btRigidBody*>(entity);

    if (!transform || !bt_rigidbody) {
        return;
    }
    btTransform bt_transform;
    (*bt_rigidbody)->getMotionState()->getWorldTransform(bt_transform);
    vec3 pos = { bt_transform.getOrigin().x(), bt_transform.getOrigin().y(),
                 bt_transform.getOrigin().z() };
    quat rot = {
        bt_transform.getRotation().w(),
        bt_transform.getRotation().x(),
        bt_transform.getRotation().y(),
        bt_transform.getRotation().z(),
    };
    transform->rotate(rot);
    vec4 translation = vec4(pos, 1) * transform->world_to_local();
    transform->set_translation(translation);
}

static void physic_system_update(World& wld)
{
    auto dynamicsWorld = wld.get<PhysicWorld>();

    if (!dynamicsWorld) {
        return;
    }
    auto rigidbody_storage = wld.get_component_storage<RigidBody>();

    if (rigidbody_storage) {
        std::for_each(
            rigidbody_storage->begin(), rigidbody_storage->end(),
            [&](auto& pair) {
                update_bullet_world_entity(
                    wld, dynamicsWorld->world(), pair.first, pair.second);
            });
    }

    dynamicsWorld->world()->stepSimulation(1.f / 120.f);

    if (rigidbody_storage) {
        std::for_each(
            rigidbody_storage->begin(), rigidbody_storage->end(),
            [&](auto& pair) {
                update_ige_entity(
                    wld, dynamicsWorld->world(), pair.first, pair.second);
            });
    }
}

void PhysicPlugin::plug(App::Builder& builder) const
{
    builder.add_startup_system(System(setup_physic_system));
    builder.add_system(System(physic_system_update));
}
