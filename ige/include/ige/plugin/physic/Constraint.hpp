#ifndef B54AF917_1DDE_440D_8098_8C3017EF8B98
#define B54AF917_1DDE_440D_8098_8C3017EF8B98

#include "ige/ecs/Entity.hpp"

#include <optional>

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige::plugin::physic {

struct Constraint {
    ecs::EntityId entity;
    glm::vec3 linearLowerLimit = glm::vec3 { 0.f };
    glm::vec3 linearUpperLimit = glm::vec3 { 0.f };
    glm::vec3 angularLowerLimit = glm::vec3 { 0.f };
    glm::vec3 angularUpperLimit = glm::vec3 { 0.f };
};
}

#endif /* B54AF917_1DDE_440D_8098_8C3017EF8B98 */
