#include "BulletGhostObject.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/physics/Collider.hpp"
#include "ige/plugin/physics/GhostObject.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using glm::mat4;
using glm::quat;
using glm::vec3;
using glm::vec4;
using ige::bt::BulletGhostObject;
using ige::ecs::EntityId;
using ige::ecs::World;
using ige::plugin::physics::Collider;
using ige::plugin::physics::ColliderType;
using ige::plugin::physics::GhostObject;
using ige::plugin::transform::Transform;

BulletGhostObject::BulletGhostObject(
    const GhostObject& object, const Transform& transform,
    std::shared_ptr<btDynamicsWorld> world)
{
    btTransform bt_transform;

    mat4 matrix
        = glm::scale(transform.local_to_world(), 1.0f / transform.scale());
    bt_transform.setFromOpenGLMatrix(glm::value_ptr(matrix));

    set_object_shape(object.collider());

    m_colShape->setLocalScaling({
        transform.scale().x,
        transform.scale().y,
        transform.scale().z,
    });
    m_object = std::make_unique<btGhostObject>();
    m_object->setCollisionShape(m_colShape.get());
    m_object->setWorldTransform(bt_transform);
    m_object->setCollisionFlags(
        m_object->getCollisionFlags()
        | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    world->addCollisionObject(m_object.get());
    world->getBroadphase()
        ->getOverlappingPairCache()
        ->setInternalGhostPairCallback(new btGhostPairCallback());
    m_world = world;
}

BulletGhostObject::~BulletGhostObject()
{
    if (!m_moved && !m_world.expired()) {
        m_world.lock()->removeCollisionObject(m_object.get());
    }
}

BulletGhostObject::BulletGhostObject(BulletGhostObject&& other)
{
    *this = std::move(other);
}

BulletGhostObject& BulletGhostObject::operator=(BulletGhostObject&& rhs)
{
    m_colShape = std::move(rhs.m_colShape);
    m_object = std::move(rhs.m_object);
    m_world = std::move(rhs.m_world);
    rhs.m_moved = true;
    return *this;
}

void BulletGhostObject::set_object_shape(const Collider& collider)
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

void BulletGhostObject::update(const Transform& transform, GhostObject&)
{
    btTransform bt_transform = m_object->getWorldTransform();

    const float* raw_matrix = glm::value_ptr(transform.local_to_world());
    bt_transform.setFromOpenGLMatrix(raw_matrix);
    m_object->setWorldTransform(bt_transform);
}

bool BulletGhostObject::operator==(const btCollisionObject* rhs)
{
    return static_cast<const btGhostObject*>(rhs) == m_object.get();
}
