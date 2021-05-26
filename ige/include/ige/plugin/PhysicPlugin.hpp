#ifndef EE588D64_EAC2_4D32_A9D4_B1F7A40648DF
#define EE588D64_EAC2_4D32_A9D4_B1F7A40648DF

#include "ige/core/App.hpp"
#include "physic/Collider.hpp"
#include "physic/PhysicWorld.hpp"
#include "physic/RigidBody.hpp"

namespace ige::plugin::physic {

class PhysicPlugin : public ige::core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* EE588D64_EAC2_4D32_A9D4_B1F7A40648DF */
