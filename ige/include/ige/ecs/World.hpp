#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/Family.hpp"
#include "ige/ecs/Table.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <span>
#include <unordered_map>
#include <vector>

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
 * @brief An Archetype groups entities having the same Family.
 *
 * The Archetype does not store component data directly. Rather, it stores a
 * pointer to the Table holding the actual data. Since some Families do not need
 * to store data, the pointer to the Table may be null.
 */
class IGE_API Archetype {
public:
    Archetype() = default;

    void merge_from(const Archetype& other);

    inline const Table* table() const noexcept { return m_table; }

    inline Table* table_mut() noexcept { return m_table; }

    inline usize entity_count() const noexcept { return m_entity_count; }

private:
    Table* m_table = nullptr;
    usize m_entity_count = 0;
};

/**
 * @brief An ArchetypeRecord is a pair of a Family and an Archetype.
 *
 * This type represents an entry in the world's archetype index.
 */
struct IGE_API ArchetypeRecord {
    const Family* family;
    Archetype* archetype;
};

/**
 * @brief A Record stores information about an Entity.
 *
 * All living entities are associated with a Record, which stores information
 * such as the Entity's archetype and its row in the Table storing the Entity's
 * components.
 */
struct IGE_API Record {
    ArchetypeRecord type;
    u64 row;
};

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

template <Component... C>
class SystemBuilder;

/**
 * @brief A world is a container for entities, components and systems.
 *
 * @see Entity
 * @see Component
 * @see Plugin
 */
class IGE_API World {
public:
    World();
    ~World();

    /**
     * @brief Create an empty entity.
     *
     * @return The created entity.
     */
    Entity entity();

    /**
     * @brief Destroy an entity.
     *
     * @param entity The entity to destroy.
     */
    void destroy(Entity entity);

    /**
     * @brief Check is an entity is alive.
     *
     * @param entity The entity to check.
     * @return True if the entity is alive, false otherwise.
     */
    bool is_alive(Entity entity) const;

    /**
     * @brief Add an attachment to an entity.
     *
     * @param entity The entity to add to.
     * @param id The attachment to add.
     */
    void add(Entity entity, u64 id);

    /**
     * @brief Attach an entity to another one.
     *
     * Equivalent to `add(entity, other.id())`.
     *
     * @param entity The entity to add to.
     * @param other The entity to add.
     */
    inline void add(Entity entity, Entity other) { add(entity, other.id()); }

    /**
     * @brief Remove an attachment from an entity.
     *
     * If the entity does not have the attachment, this function does nothing.
     *
     * @param entity The entity to remove from.
     * @param id The attachment to remove.
     */
    void remove(Entity entity, u64 id);

    /**
     * @brief Detach an entity from another one.
     *
     * Equivalent to `remove(entity, other.id())`.
     *
     * @param entity The entity to remove from.
     * @param other The entity to remove.
     */
    inline void remove(Entity entity, Entity other)
    {
        remove(entity, other.id());
    }

    /**
     * @brief Check if an entity has an attachment.
     *
     * @param entity The entity to check.
     * @param id The attachment to check.
     * @return True if the entity has the attachment, false otherwise.
     */
    bool has(Entity entity, u64 id) const;

    /**
     * @brief Check if an entity has another one.
     *
     * Equivalent to `has(entity, other.id())`.
     *
     * @param entity The entity to check.
     * @param other The entity to check.
     * @return True if the entity has the attachment, false otherwise.
     */
    inline bool has(Entity entity, Entity other) const
    {
        return has(entity, other.id());
    }

    /**
     * @brief Create a new query.
     */
    Query<> query();

    /**
     * @brief Create a system and add it to this world.
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

    /**
     * @brief Spawn a new entity.
     *
     * @param components A list of components to be added to the new entity.
     * @return Entity ID of the new entity.
     */
    template <Component... Cs>
    Entity spawn(Cs&&...);

    /**
     * @brief Load a plugin.
     *
     * If the plugin was already loaded to this world, no action is taken.
     *
     * @tparam P The plugin type.
     * @return bool true if the plugin was loaded, false if it was already.
     */
    template <Plugin P>
    bool load();

#if 0
    /**
     * @brief Add components to an entity.
     *
     * If any of the components are already present on the entity, they are
     * replaced.
     *
     * @param entity The entity to add the components to.
     * @param components The components to add.
     */
    template <Component... Cs>
    void add(Entity, Cs&&...);

    /**
     * @brief Remove components from an entity.
     *
     * If any of the components are not present on the entity, no action is
     * taken.
     *
     * @param entity The entity to remove the components from.
     * @param components The components to remove.
     */
    template <Component... Cs>
    void remove(Entity, Cs&&...);

    /**
     * @brief Get a constant pointer to a component attached to the given
     * entity.
     *
     * If no component of the specified type exists, a null pointer is returned.
     *
     * @param entity The entity.
     * @return const C* A pointer to the component.
     */
    template <Component C>
    const C* get(Entity) const;

    /**
     * @brief Set the value of one or more components attached to the given
     * entity.
     *
     * If any component is missing from the entity, it is added.
     *
     * @tparam Cs The component types.
     * @param entity The entity.
     * @param components The component values to set.
     */
    template <Component... Cs>
    void set(Entity, Cs&&...);

    /**
     * @brief Get a constant pointer to a resource.
     *
     * If no resource of the specified type exists, a null pointer is returned.
     *
     * @tparam C The resource type.
     * @return const C* A pointer to the resource.
     */
    template <Component C>
    const C* get() const;
#endif

private:
    using EntityList = std::vector<Entity>;
    using EntityIndex = std::unordered_map<u64, Record>;
    using ArchetypeIndex
        = std::unordered_map<Family, Archetype, Family::Hash, std::equal_to<>>;

    /**
     * @brief Get a reference to the empty Family.
     */
    ArchetypeRecord empty_archetype();

    /**
     * @brief Get a reference to the archetype representing the given set of
     * attachments.
     *
     * The archetype will be created if it doesn't exist.
     */
    ArchetypeRecord get_archetype(std::span<const u64>);

    /**
     * @brief Get a reference to the archetype for the given descriptor.
     *
     * The archetype will be created if it doesn't exist.
     */
    ArchetypeRecord get_archetype(Family::With);

    /**
     * @brief Get a reference to the archetype for the given descriptor.
     *
     * The archetype will be created if it doesn't exist.
     */
    ArchetypeRecord get_archetype(Family::Without);

    u32 m_tick = 0;
    u32 m_last_entity_id = 0;
    EntityList* m_free_entities = nullptr;
    EntityIndex* m_entity_index = nullptr;
    ArchetypeIndex* m_archetypes = nullptr;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
