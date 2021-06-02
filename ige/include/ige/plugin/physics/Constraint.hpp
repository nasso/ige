#ifndef B54AF917_1DDE_440D_8098_8C3017EF8B98
#define B54AF917_1DDE_440D_8098_8C3017EF8B98

#include "ige/ecs/Entity.hpp"
#include <glm/vec3.hpp>

namespace ige::plugin::physics {

struct Constraint {
    ecs::EntityId entity;
    glm::vec3 linear_lower_limit { 0.0f };
    glm::vec3 linear_upper_limit { 0.0f };
    glm::vec3 angular_lower_limit { 0.0f };
    glm::vec3 angular_upper_limit { 0.0f };
};

}

#endif /* B54AF917_1DDE_440D_8098_8C3017EF8B98 */
