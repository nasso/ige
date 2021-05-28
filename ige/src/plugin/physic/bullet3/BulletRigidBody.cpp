#include "BulletRigidBody.hpp"
#include "ige.hpp"

#include <iostream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::bt::BulletRigidBody;
using ige::ecs::EntityId;
using ige::ecs::World;
using ige::plugin::physic::Collider;
using ige::plugin::physic::ColliderType;
using ige::plugin::physic::RigidBody;
using ige::plugin::physic::RigidBodyStatus;
using ige::plugin::transform::Transform;

BulletRigidBody::BulletRigidBody(
    const RigidBody& rigidbody, const Transform& transform,
    btDynamicsWorld* world)
{
    btTransform bt_transform;

    const float* raw_matrix = (const float*)glm::value_ptr(
        transform.local_to_world()
        * glm::scale(mat4(1.0f), 1.0f / transform.scale()));
    bt_transform.setFromOpenGLMatrix(raw_matrix);
    m_motion_state = std::unique_ptr<btDefaultMotionState>(
        new btDefaultMotionState(bt_transform));

    set_rigibody_shape(rigidbody.collider());
    btVector3 inertia(0.f, 0.f, 0.f);

    m_colShape->setLocalScaling({
        transform.scale().x,
        transform.scale().y,
        transform.scale().z,
    });
    if (rigidbody.get_mass() != 0.f) {
        m_colShape->calculateLocalInertia(rigidbody.get_mass(), inertia);
    }
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
        rigidbody.get_mass(), m_motion_state.get(), m_colShape.get(), inertia);
    m_rigidbody = std::make_unique<btRigidBody>(rbInfo);
    if (rigidbody.is_kinematic()) {
        m_rigidbody->setCollisionFlags(
            m_rigidbody->getCollisionFlags()
            | btRigidBody::CF_KINEMATIC_OBJECT);
    }
    world->addRigidBody(m_rigidbody.get());
}

void BulletRigidBody::set_rigibody_shape(const Collider& collider)
{
    switch (collider.type) {
    case ColliderType::BOX:
        m_colShape = std::make_unique<btBoxShape>(
            btVector3 { collider.box.x, collider.box.y, collider.box.z });
        break;
    case ColliderType::SPHERE:
        m_colShape = std::make_unique<btSphereShape>(collider.sphere.radius);
        break;
    case ColliderType::CAPSULE:
        m_colShape = std::make_unique<btCapsuleShape>(
            collider.capsule.radius, collider.capsule.height);
    default:
        return;
    }
}

void BulletRigidBody::update(World& wld, EntityId entity)
{
    auto transform = wld.get_component<Transform>(entity);
    auto rigidbody = wld.get_component<RigidBody>(entity);

    if (!transform || !rigidbody) {
        return;
    }
    if (rigidbody->is_kinematic()) {
        btTransform bt_transform;

        m_motion_state->getWorldTransform(bt_transform);

        const float* raw_matrix
            = (const float*)glm::value_ptr(transform->local_to_world());
        bt_transform.setFromOpenGLMatrix(raw_matrix);
        m_motion_state->setWorldTransform(bt_transform);
    }
    if (rigidbody->status() == RigidBodyStatus::DIRTY) {
        rigidbody->update();
        const auto& forces = rigidbody->get_forces();
        for (vec3 force : forces) {
            m_rigidbody->applyCentralImpulse(
                btVector3(force.x, force.y, force.z));
        }
        rigidbody->clear_forces();
    }
}

void BulletRigidBody::sync_ige_entity(World& wld, EntityId entity)
{
    auto transform = wld.get_component<Transform>(entity);

    if (!transform) {
        return;
    }
    btTransform bt_transform;
    m_motion_state->getWorldTransform(bt_transform);

    float matrix_raw[16];
    bt_transform.getOpenGLMatrix(matrix_raw);
    mat4 matrix = glm::make_mat4x4(matrix_raw);

    vec3 translation;
    quat rotation;
    vec3 scale;
    vec3 skew;
    vec4 perspective;
    if (!glm::decompose(
            matrix, scale, rotation, translation, skew, perspective)) {
        std::cerr << "[WARN] Couldn't decompose node matrix." << std::endl;
    } else {
        transform->set_translation(translation);
        transform->set_rotation(rotation);
    }
}

bool BulletRigidBody::operator==(const BulletRigidBody& rhs)
{
    return rhs.m_rigidbody == m_rigidbody;
}

bool BulletRigidBody::operator==(const btRigidBody* rhs)
{
    return rhs == m_rigidbody.get();
}