#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "Entity.hpp"
#include <concepts>

namespace ige::core {

template <class T>
concept Tag = std::is_empty_v<T>;

/**
 * @brief Concept for a Component.
 *
 * Components must be "semiregular" (i.e. they must have a default constructor,
 * a copy constructor, a move constructor, a copy assignment operator, a move
 * assignment operator, and a destructor).
 */
template <class C>
concept Component = Tag<C> || std::semiregular<C>;

class World;

/**
 * @brief Concept defining a Plugin.
 *
 * A plugin is any type constructible from a lvalue reference to a World, and
 * satisfying the requirements of the concept for Component.
 */
template <class P>
concept Plugin = Component<P> && std::constructible_from<P, ige::core::World&>;

/**
 * @brief A world is a container for entities, components and systems.
 *
 * @see Entity
 * @see Component
 * @see Plugin
 */
class World {
public:
    /**
     * @brief Spawn a new entity.
     *
     * @return Entity ID of the new entity.
     */
    template <Component... Cs>
    Entity spawn(Cs...);

    /**
     * @brief Load a plugin.
     *
     * If the plugin has already been loaded, no action is taken.
     */
    template <Plugin P>
    bool load();
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
