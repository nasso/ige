#ifndef C70372BC_9E7E_46A5_B19C_8A7750BC1EFD
#define C70372BC_9E7E_46A5_B19C_8A7750BC1EFD

#include "Collider.hpp"

#include <vector>

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige::plugin::physic {

enum class RigidBodyStatus { CLEAN, DIRTY };

class RigidBody {
public:
    RigidBody(Collider collider, int mass = 1, bool is_kinematic = false);

    RigidBodyStatus status() const;
    Collider collider() const;

    RigidBody& is_kinematic(bool is_kinematic);
    bool is_kinematic() const;

    void mass(float mass);
    float mass() const;

    bool use_gravity() const;
    void use_gravity(bool use_gravity);

    RigidBody& apply_force(glm::vec3 force);
    const std::vector<glm::vec3>& get_forces() const;
    void clear_forces();

    void velocity(glm::vec3 velocity);
    glm::vec3 velocity() const;

    void update();

    bool freeze_rotation() const;
    void freeze_rotation(bool free_rotation);

    bool freeze_position() const;
    void freeze_position(bool free_position);

    glm::vec3 center_of_mass() const;
    void center_of_mass(glm::vec3 center_of_mass);
    void reset_center_of_mass();

private:
    std::vector<glm::vec3> m_forces;
    glm::vec3 m_center_of_mass = glm::vec3 { 0.f };
    glm::vec3 m_velocity = glm::vec3 { 0.f };
    float m_mass;
    bool m_is_kinematic;
    bool m_use_gravity = false;
    bool m_freeze_rotation = false;
    bool m_freeze_position = false;

    RigidBodyStatus m_status;
    Collider m_collider;
};
}

#endif /* C70372BC_9E7E_46A5_B19C_8A7750BC1EFD */
