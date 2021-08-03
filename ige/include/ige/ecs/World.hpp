#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/core/Platform.hpp"
#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include <concepts>

namespace ige::ecs {

class World;

/**
 * @brief Concept defining a Plugin.
 *
 * A plugin is any type constructible from a lvalue reference to a World, and
 * satisfying the requirements of the concept for Component.
 */
template <class P>
concept Plugin = Component<P> && std::constructible_from<P, ige::ecs::World&>;

/**
 * @brief A world is a container for entities, components and systems.
 *
 * @see Entity
 * @see Component
 * @see Plugin
 */
class IGE_API World {
public:
    void update();

    template <Component... Cs>
    Entity spawn(Cs&&...);

    template <Plugin P>
    bool load();

    template <Component C>
    const C* get() const;

    template <Component C>
    C* get_mut() const;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
