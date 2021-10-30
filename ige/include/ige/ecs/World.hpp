#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <concepts>
#include <functional>
#include <vector>

namespace ige::ecs {

template <Component... C>
class SystemBuilder;

class World;

/**
 * @brief An Archetype regroups entities having the same attachments.
 */
struct Archetype {
    std::vector<u64> attachments;
};

/**
 * @brief Concept defining a Plugin.
 *
 * A plugin is any type constructible from a lvalue reference to a World, and
 * satisfying the requirements of the concept for Component.
 */
template <class P>
concept Plugin = Component<P> && std::constructible_from<P, ige::ecs::World&>;

/**
 * @brief A Query is used to rapidly find and iterate a set of entities matching
 * a logical expression defining what attachments each entity must and must not
 * have.
 *
 * @see World::query
 */
template <Component... Cs>
class Query {
public:
    explicit Query(World& world)
        : m_world(world)
    {
    }

    // TODO: union<Cs..., Css...> to remove duplicates

    template <Component... Css>
    Query<Cs..., Css...> all();

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
     * @brief Create an empty entity.
     */
    Entity empty();

    /**
     * @brief Add an attachment to an entity.
     */
    void attach(Entity, u64);

    /**
     * @brief Remove an attachment from an entity.
     */
    void detach(Entity, u64);

    /**
     * @brief Create a new query.
     */
    Query<> query();

    /**
     * @brief Create a system and attach it to this world.
     *
     * @param f The system's run function.
     */
    SystemBuilder<> system();

    /**
     * @brief Run one step of the simulation.
     *
     * Executes all scheduled systems.
     */
    void update();

    template <Component... Cs>
    Entity spawn(Cs&&...);

    template <Plugin P>
    bool load();

    template <Component... Cs>
    void add(Entity, Cs&&...);

    template <Component... Cs>
    void remove(Entity, Cs&&...);

    template <Component C>
    const C* get(Entity) const;

    template <Component... Cs>
    void set(Entity, Cs&&...);

#if IGE_WORLD_RESOURCE_API
    template <Component C>
    const C* get() const;
#endif

private:
    u32 m_tick = 0;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
