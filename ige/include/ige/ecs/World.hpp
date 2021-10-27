#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <concepts>
#include <functional>

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
 * @brief SystemBuilders are temporary objects used when constructing a system.
 *
 * @see World::system
 */
template <Component... Cs>
class SystemBuilder {
public:
    explicit SystemBuilder(World& world)
        : m_world(world)
    {
    }

    // TODO: union<Cs..., Css...> to remove duplicates

    template <Component... Css>
    SystemBuilder<Cs..., Css...> all();

    template <std::invocable<Cs&...> F>
    void each(F&& f);

private:
    World& m_world;
};

/**
 * @brief A world is a container for entities, components and systems.
 *
 * @see Entity
 * @see Component
 * @see Plugin
 */
class IGE_API World {
public:
    /**
     * @brief Run one step of the simulation.
     *
     * Executes all scheduled systems.
     */
    void update();

    /**
     * @brief Create a system and attach it to this world.
     *
     * @param f The system's run function.
     */
    SystemBuilder<> system();

    template <Component... Cs>
    Entity spawn(Cs&&...);

    template <Plugin P>
    bool load();

    template <Component C>
    const C* get() const;

    template <Component C>
    C* get_mut();

    template <Component C>
    const C* get(Entity) const;

    template <Component C>
    C* get_mut(Entity);

private:
    u32 m_tick = 0;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
