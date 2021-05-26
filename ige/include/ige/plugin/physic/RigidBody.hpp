#ifndef C70372BC_9E7E_46A5_B19C_8A7750BC1EFD
#define C70372BC_9E7E_46A5_B19C_8A7750BC1EFD

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige {
namespace plugin {
    namespace physic {

        enum class RigidBodyStatus { NEW, CLEAN, DIRTY };

        class RigidBody {
        public:
            RigidBody(int mass = 1, bool is_kinematic = false);

            RigidBodyStatus status() const;

            RigidBody& is_kinematic(bool is_kinematic);
            bool is_kinematic() const;

            RigidBody& set_mass(float mass);
            float get_mass() const;

            RigidBody& apply_force(float force);
            float apply_force();

            void initialize();
            void update();

        private:
            float m_force = 0.f;
            float m_mass;
            bool m_is_kinematic;

            RigidBodyStatus m_status;
        };
    }
}
}

#endif /* C70372BC_9E7E_46A5_B19C_8A7750BC1EFD */
