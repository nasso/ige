#ifndef EE588D64_EAC2_4D32_A9D4_B1F7A40648DF
#define EE588D64_EAC2_4D32_A9D4_B1F7A40648DF

#include "ige/core/App.hpp"
#include "physics/Collider.hpp"
#include "physics/Constraint.hpp"
#include "physics/GhostObject.hpp"
#include "physics/PhysicsWorld.hpp"
#include "physics/RigidBody.hpp"

namespace ige::plugin::physics {

class PhysicsPlugin : public ige::core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* EE588D64_EAC2_4D32_A9D4_B1F7A40648DF */
