#ifndef B54AF917_1DDE_440D_8098_8C3017EF8B98
#define B54AF917_1DDE_440D_8098_8C3017EF8B98

#include "ige/ecs/Entity.hpp"
#include <glm/vec3.hpp>
#include <optional>

namespace ige::plugin::physic {

struct Constraint {
    ecs::EntityId entity;
    glm::vec3 linearLowerLimit { 0.0f };
    glm::vec3 linearUpperLimit { 0.0f };
    glm::vec3 angularLowerLimit { 0.0f };
    glm::vec3 angularUpperLimit { 0.0f };
};

}

#endif /* B54AF917_1DDE_440D_8098_8C3017EF8B98 */
