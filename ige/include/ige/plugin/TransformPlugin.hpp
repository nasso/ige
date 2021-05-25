#ifndef F4DF8A5F_1CCD_443F_8D71_8A439340E94F
#define F4DF8A5F_1CCD_443F_8D71_8A439340E94F

#include "ige/ecs/VecStorage.hpp"
#include "ige/ecs/World.hpp"
#include <optional>
#include <unordered_set>
#include <vector>

#include <glm/ext/quaternion_float.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige::plugin::transform {

/**
 * @brief Component giving a parent to an entity.
 */
struct Parent {
    ecs::EntityId entity;

    Parent(ecs::EntityId parent_entity);

    bool operator==(const Parent&) const = default;
};

/**
 * @brief Component holding a list of entities that have this entity as their
 * parent.
 *
 * This component is automatically added and updated by the parenting system.
 * Any modification made to it will be overwritten by the engine!
 */
struct Children {
    std::unordered_set<ecs::EntityId> entities;
};

class Transform {
private:
    glm::vec3 m_translation = glm::vec3(0.0f);
    glm::quat m_rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 m_scale = glm::vec3(1.0f);

    bool m_dirty = false;
    glm::mat4 m_local_to_world { 1.0f };
    glm::mat4 m_world_to_local { 1.0f };

public:
    using Storage = ecs::VecStorage<Transform>;

    static Transform from_pos(glm::vec3 position);

    constexpr explicit Transform() = default;

    glm::vec3 translation() const;
    glm::quat rotation() const;
    glm::vec3 scale() const;

    Transform& set_translation(glm::vec3) &;
    Transform& set_rotation(glm::quat) &;
    Transform& set_scale(glm::vec3) &;
    Transform& set_scale(float) &;

    Transform& translate(glm::vec3);
    Transform& rotate(glm::vec3);
    Transform& rotate(glm::quat);
    Transform& scale(glm::vec3);

    Transform&
    look_at(glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) &;

    Transform set_translation(glm::vec3) &&;
    Transform set_rotation(glm::quat) &&;
    Transform set_scale(glm::vec3) &&;
    Transform set_scale(float) &&;
    Transform
    look_at(glm::vec3 target, glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f)) &&;

    void force_update(const glm::mat4& parent);
    bool needs_update() const;

    const glm::mat4& local_to_world() const;
    const glm::mat4& world_to_local() const;
};

class TransformPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* F4DF8A5F_1CCD_443F_8D71_8A439340E94F */
