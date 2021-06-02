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

    Collider collider() const;

    RigidBody& set_kinematic(bool is_kinematic = true);
    bool is_kinematic() const;

    RigidBody& set_mass(float mass);
    float mass() const;

    RigidBody& set_use_gravity(bool use_gravity);
    bool use_gravity() const;

    void apply_force(const glm::vec3& force);
    const glm::vec3& get_forces() const;
    void clear_forces();

    RigidBody& set_velocity(const glm::vec3& velocity);
    const glm::vec3& velocity() const;

    RigidBody& set_freeze_rotation(bool freeze_rotation);
    bool freeze_rotation() const;

    RigidBody& set_freeze_position(bool freeze_position);
    bool freeze_position() const;

    RigidBody& set_center_of_mass(const glm::vec3& center_of_mass);
    const glm::vec3& center_of_mass() const;
    void reset_center_of_mass();

private:
    Collider m_collider;
    float m_mass;
    bool m_is_kinematic;
    glm::vec3 m_forces;
    glm::vec3 m_center_of_mass = glm::vec3 { 0.0f };
    glm::vec3 m_velocity = glm::vec3 { 0.0f };
    bool m_use_gravity = true;
    bool m_freeze_rotation = false;
    bool m_freeze_position = false;

    RigidBodyStatus m_status;
};

}

#endif /* C70372BC_9E7E_46A5_B19C_8A7750BC1EFD */
