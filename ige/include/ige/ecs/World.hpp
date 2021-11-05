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
#include <optional>
#include <span>
#include <unordered_map>
#include <vector>

namespace ige::ecs {

/**
 * @brief Structure holding runtime type information about a component.
 */
struct IGE_API TypeInfo {
    usize size;
    usize align;
};

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
struct IGE_API Archetype {
    Table* table = nullptr;
};

/**
 * @brief An ArchetypeRecord is a pair of a Family and an Archetype.
 *
 * This type represents an entry in the world's archetype index.
 */
class IGE_API ArchetypeRecord {
public:
    ArchetypeRecord(const Family& family, Archetype& archetype)
        : m_family(&family)
        , m_archetype(&archetype) {};
    ArchetypeRecord(const ArchetypeRecord&) = default;
    ArchetypeRecord(ArchetypeRecord&&) = default;
    ArchetypeRecord& operator=(const ArchetypeRecord&) = default;
    ArchetypeRecord& operator=(ArchetypeRecord&&) = default;

    inline const Family& family() const noexcept { return *m_family; }
    inline const Archetype& archetype() const noexcept { return *m_archetype; }
    inline Archetype& archetype_mut() noexcept { return *m_archetype; }

private:
    const Family* m_family;
    Archetype* m_archetype;
};

/**
 * @brief A Record stores information about an Entity.
 *
 * All living entities are associated with a Record, which stores information
 * such as the Entity's archetype and its row in the Table storing the Entity's
 * components.
 */
class IGE_API Record {
public:
    Record(ArchetypeRecord type, usize row)
        : m_type(type)
        , m_row(row) {};
    Record(const Record&) = default;
    Record(Record&&) = default;
    Record& operator=(const Record&) = default;
    Record& operator=(Record&&) = default;

    inline const ArchetypeRecord& type() const noexcept { return m_type; }
    inline ArchetypeRecord& type_mut() noexcept { return m_type; }
    inline const usize& row() const noexcept { return m_row; }
    inline usize& row_mut() noexcept { return m_row; }

    inline const Family& family() const noexcept { return m_type.family(); }

    inline const Archetype& archetype() const noexcept
    {
        return m_type.archetype();
    }

    inline Archetype& archetype_mut() noexcept
    {
        return m_type.archetype_mut();
    }

private:
    ArchetypeRecord m_type;
    usize m_row;
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
     * @brief Create a component.
     *
     * @param size The size of the component data in bytes.
     * @param align The alignment of the component data in bytes.
     * @return An entity representing the component.
     */
    Entity component(usize size, usize align);

    /**
     * @brief Set the data of a component.
     *
     * @param entity The entity having the component.
     * @param component The component to set.
     * @param data The data to set.
     */
    void set(Entity entity, Entity component, const void* data);

    /**
     * @brief Mutate the data of a component using a pure function.
     *
     * @param entity The entity having the component.
     * @param component The component to set.
     * @param f The function to apply to the data.
     */
    template <std::regular_invocable<void*> F>
    void mutate(Entity entity, Entity component, F&& f);

    /**
     * @brief Get the data of a component.
     *
     * If the entity does not have the component, this function returns nullptr.
     *
     * @param entity The entity having the component.
     * @param component The component to get.
     * @return A pointer to the component data.
     */
    const void* get(Entity entity, Entity component) const;

    /**
     * @brief Check whether the given ID represents a Component.
     */
    bool is_component(u64) const;

    /**
     * @brief Check whether the given Entity is a Component.
     */
    inline bool is_component(Entity entity) const
    {
        return is_component(entity.id());
    }

    /**
     * @brief Run one step of the simulation.
     *
     * Executes all scheduled systems.
     */
    void update();

    // TEMPLATED INTERFACE

    /**
     * @brief Create a new component from a C++ type.
     *
     * This function always creates a new component.
     *
     * @tparam C The C++ type of the component.
     * @return The Entity representing the component.
     */
    template <Component C>
    Entity component();

    /**
     * @brief Get or create a component associated with a C++ type.
     *
     * On its first call, an instance of this method template creates a new
     * component and associates it with the given C++ type. Subsequent calls
     * will always return the same component.
     *
     * @tparam C The C++ type of the component.
     * @return The Entity representing the component.
     */
    template <Component C>
    Entity static_component();

    /**
     * @brief Get a component associated with a C++ type.
     *
     * Return the Entity representing the component previously registered with
     * `static_component<C>()`. If the component wasn't registered yet, this
     * function returns std::nullopt.
     *
     * @tparam C The C++ type of the component.
     * @return The Entity representing the component.
     */
    template <Component C>
    std::optional<Entity> get_static_component() const;

    /**
     * @brief Set the data of a component.
     *
     * @tparam C The C++ type identifying the component.
     * @param entity The entity.
     * @param data The component data to set.
     */
    template <Component C>
    void set(Entity entity, const C& data);

    /**
     * @brief Get the data of a component identified by its C++ type.
     *
     * If the entity does not have the component, this function returns nullptr.
     *
     * @tparam C The C++ type identifying the component.
     * @param entity The entity.
     * @return const C* A pointer to the component.
     */
    template <Component C>
    const C* get(Entity entity) const;

    /**
     * @brief Remove components from an entity.
     *
     * Components not attached to the entity are silently ignored.
     *
     * @tparam C The first component to remove.
     * @tparam Cs The remaining components to remove.
     * @param entity The entity to remove the components from.
     */
    template <Component C, Component... Cs>
    void remove(Entity entity);

    /**
     * @brief Create a new query.
     *
     * @tparam Cs The components to query for.
     * @return A query for the given components.
     */
    template <Component... Cs>
    Query<Cs...> query();

    /**
     * @brief Create a system and add it to this world.
     *
     * @tparam Cs The components to query for.
     * @return A system builder to configure the system.
     */
    template <Component... Cs>
    SystemBuilder<Cs...> system();

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
    using StaticComponentIndex = std::unordered_map<u64, Entity>;
    using ArchetypeIndex
        = std::unordered_map<Family, Archetype, Family::Hash, std::equal_to<>>;

    /**
     * @brief Create an Entity with the given Record.
     *
     * @return The new Entity.
     */
    Entity entity(Record);

    /**
     * @brief Get or create the Table for the given ArchetypeRecord.
     *
     * If the Archetype doesn't store any component, nullptr is returned.
     */
    Table* touch_table(ArchetypeRecord&);

    /**
     * @brief Get the type information for a component.
     *
     * @param component The component entity.
     */
    const TypeInfo* get_component_type_info(Entity component) const;

    /**
     * @brief Create or set a Record for the given Entity.
     */
    void set_record(Entity, Record);

    /**
     * @brief Get a constant pointer to the Record of the given Entity.
     */
    const Record* get_record(Entity) const;

    /**
     * @brief Get a mutable pointer to the Record of the given Entity.
     */
    Record* get_record_mut(Entity);

    /**
     * @brief Add an attachment to an entity.
     */
    void add_to_record(Record&, u64);

    /**
     * @brief Remove an attachment from an entity.
     */
    void remove_from_record(Record&, u64);

    /**
     * @brief Get the column storing the data of the given component for the
     * given Family.
     *
     * If the given identifier does not name a component, std::nullopt is
     * returned.
     */
    std::optional<usize> get_component_column(const Family&, u32) const;

    /**
     * @brief Get the column storing the data of the given component for the
     * given Family.
     *
     * If the given identifier does not name a component, std::nullopt is
     * returned.
     */
    inline std::optional<usize>
    get_component_column(const Family& fam, Entity entity) const
    {
        return get_component_column(fam, entity.id());
    }

    /**
     * @brief Change a Record's archetype.
     *
     * Components are moved from the old archetype to the new one. If the new
     * archetype is larger than the old one, the new components are left
     * uninitialized. If the new archetype is smaller than the old one, the
     * excess components are discarded. If the new archetype is the same as the
     * old one, no action is taken.
     */
    void migrate_record(ArchetypeRecord&, Record&);

    /**
     * @brief Move all entities from one archetype to another.
     *
     * @param dst The destination archetype record.
     * @param src The source archetype record.
     */
    void migrate_all(ArchetypeRecord& dst, const ArchetypeRecord& src);

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
    StaticComponentIndex* m_static_component_index = nullptr;
    Entity m_comp_type_info = entity();
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
