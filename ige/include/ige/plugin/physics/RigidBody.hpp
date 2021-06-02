#ifndef C70372BC_9E7E_46A5_B19C_8A7750BC1EFD
#define C70372BC_9E7E_46A5_B19C_8A7750BC1EFD

#include "Collider.hpp"
#include <glm/vec3.hpp>

namespace ige::plugin::physics {

enum class RigidBodyStatus { CLEAN, DIRTY };

class RigidBody {
public:
    RigidBody(Collider collider, float mass = 1, bool is_kinematic = false);

    bool is_dirty() const;
    void clean();

    const Collider& collider() const;
    bool is_kinematic() const;
    float mass() const;
    bool use_gravity() const;
    bool freeze_rotation() const;
    bool freeze_position() const;
    const glm::vec3& get_forces() const;
    const glm::vec3& center_of_mass() const;
    const glm::vec3& velocity() const;

    void apply_force(const glm::vec3& force);
    void clear_forces();
    void reset_center_of_mass();

    RigidBody& set_kinematic(bool is_kinematic = true) &;
    RigidBody& set_mass(float mass) &;
    RigidBody& set_use_gravity(bool use_gravity) &;
    RigidBody& set_velocity(const glm::vec3& velocity) &;
    RigidBody& set_freeze_rotation(bool freeze_rotation) &;
    RigidBody& set_freeze_position(bool freeze_position) &;
    RigidBody& set_center_of_mass(const glm::vec3& center_of_mass) &;

    RigidBody set_kinematic(bool is_kinematic = true) &&;
    RigidBody set_mass(float mass) &&;
    RigidBody set_use_gravity(bool use_gravity) &&;
    RigidBody set_velocity(const glm::vec3& velocity) &&;
    RigidBody set_freeze_rotation(bool freeze_rotation = true) &&;
    RigidBody set_freeze_position(bool freeze_position = true) &&;
    RigidBody set_center_of_mass(const glm::vec3& center_of_mass) &&;

private:
    Collider m_collider;
    float m_mass;
    bool m_is_kinematic;
    glm::vec3 m_forces = glm::vec3 { 0.0f };
    glm::vec3 m_center_of_mass = glm::vec3 { 0.0f };
    glm::vec3 m_velocity = glm::vec3 { 0.0f };
    bool m_use_gravity = true;
    bool m_freeze_rotation = false;
    bool m_freeze_position = false;

    RigidBodyStatus m_status;
};

}

#endif /* C70372BC_9E7E_46A5_B19C_8A7750BC1EFD */
