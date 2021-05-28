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

    RigidBody& set_mass(float mass);
    float get_mass() const;

    RigidBody& apply_force(glm::vec3 force);
    const std::vector<glm::vec3>& get_forces() const;
    void clear_forces();

    void update();

private:
    std::vector<glm::vec3> m_forces;
    float m_mass;
    bool m_is_kinematic;

    RigidBodyStatus m_status;
    Collider m_collider;
};
}

#endif /* C70372BC_9E7E_46A5_B19C_8A7750BC1EFD */
