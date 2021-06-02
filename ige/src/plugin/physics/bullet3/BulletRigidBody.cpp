#include "BulletRigidBody.hpp"
#include "ige.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::bt::BulletRigidBody;
using ige::ecs::EntityId;
using ige::ecs::World;
using ige::plugin::physics::Collider;
using ige::plugin::physics::ColliderType;
using ige::plugin::physics::RigidBody;
using ige::plugin::physics::RigidBodyStatus;
using ige::plugin::transform::Transform;

BulletRigidBody::BulletRigidBody(
    const RigidBody& rigidbody, const Transform& transform,
    btDynamicsWorld* world)
{
    btTransform bt_transform;

    mat4 matrix
        = glm::scale(transform.local_to_world(), 1.0f / transform.scale());
    bt_transform.setFromOpenGLMatrix(glm::value_ptr(matrix));
    m_motion_state = std::make_unique<btDefaultMotionState>(bt_transform);

    set_rigibody_shape(rigidbody.collider());
    btVector3 inertia(0.f, 0.f, 0.f);

    m_colShape->setLocalScaling({
        transform.scale().x,
        transform.scale().y,
        transform.scale().z,
    });
    if (rigidbody.mass() != 0.f) {
        m_colShape->calculateLocalInertia(rigidbody.mass(), inertia);
    }
    btRigidBody::btRigidBodyConstructionInfo rbInfo(
        rigidbody.mass(), m_motion_state.get(), m_colShape.get(), inertia);
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
        m_colShape = std::make_unique<btBoxShape>(btVector3 {
            collider.box.extents.x / 2.0f,
            collider.box.extents.y / 2.0f,
            collider.box.extents.z / 2.0f,
        });
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

void BulletRigidBody::update(const Transform& transform, RigidBody& rigidbody)
{
    if (rigidbody.is_kinematic()) {
        btTransform bt_transform;

        m_motion_state->getWorldTransform(bt_transform);

        const float* raw_matrix = glm::value_ptr(transform.local_to_world());
        bt_transform.setFromOpenGLMatrix(raw_matrix);
        m_motion_state->setWorldTransform(bt_transform);
    }

    if (rigidbody.is_dirty()) {
        m_rigidbody->activate();
        m_rigidbody->setAngularFactor(
            rigidbody.freeze_rotation() ? 0.0f : 1.0f);
        m_rigidbody->setLinearFactor(
            rigidbody.freeze_position() ? btVector3 { 0.0f, 0.0f, 0.0f }
                                        : btVector3 { 1.0f, 1.0f, 1.0f });

        glm::vec3 velocity = rigidbody.velocity();
        m_rigidbody->setLinearVelocity({ velocity.x, velocity.y, velocity.z });

        const auto& force = rigidbody.get_forces();
        m_rigidbody->applyCentralImpulse({ force.x, force.y, force.z });
        rigidbody.clear_forces();

        auto bt_center_of_mass = m_rigidbody->getCenterOfMassTransform();
        const auto& center_of_mass = rigidbody.center_of_mass();
        bt_center_of_mass.setOrigin({
            bt_center_of_mass.getOrigin().x() + center_of_mass.x,
            bt_center_of_mass.getOrigin().y() + center_of_mass.y,
            bt_center_of_mass.getOrigin().z() + center_of_mass.z,
        });
        m_rigidbody->setCenterOfMassTransform(bt_center_of_mass);

        if (rigidbody.use_gravity()) {
            btVector3 inertia;

            m_colShape->calculateLocalInertia(rigidbody.mass(), inertia);
            m_rigidbody->setMassProps(rigidbody.mass(), inertia);
        } else {
            btVector3 inertia;

            m_colShape->calculateLocalInertia(0, inertia);
            m_rigidbody->setMassProps(0, inertia);
        }
        rigidbody.clean();
    }
}

void BulletRigidBody::sync_ige_entity(World& wld, EntityId entity)
{
    auto transform = wld.get_component<Transform>(entity);
    auto rigidbody = wld.get_component<RigidBody>(entity);

    if (transform && rigidbody) {
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
            std::cerr << "[ERROR] Couldn't decompose node matrix." << std::endl;
        } else {
            transform->set_translation(translation);
            transform->set_rotation(rotation);
        }

        btVector3 velocity = m_rigidbody->getLinearVelocity();
        rigidbody->set_velocity(
            vec3 { velocity.x(), velocity.y(), velocity.z() });
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

btRigidBody* BulletRigidBody::body()
{
    return m_rigidbody.get();
}
