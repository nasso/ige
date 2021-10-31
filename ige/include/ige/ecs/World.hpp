#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/Table.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <algorithm>
#include <concepts>
#include <functional>
#include <span>
#include <vector>

namespace ige::ecs {

/**
 * @brief A Family represents a unique set of attachments.
 *
 * Every Entity in the World belongs to exactly one Family. The (unordered) set
 * of attachments an Entity has uniquely determines its Family.
 */
class IGE_API Family {
public:
    /**
     * @brief Create a new Family.
     *
     * @param ids The set of attachment ids.
     */
    Family(std::span<const u64> ids);

    Family(const Family&) = delete;
    Family& operator=(const Family&) = delete;
    Family(Family&&);
    Family& operator=(Family&&);

    Family clone() const;

    ~Family();

    /**
     * @brief Get the set of attachment ids.
     *
     * @return The set of attachment ids.
     */
    std::span<const u64> ids() const;

    /**
     * @brief Get the number of attachments in the Family.
     *
     * @return The number of attachments in the Family.
     */
    usize size() const;

    /**
     * @brief Compare this Family to a set of attachments.
     *
     * @param ids The set of attachment ids.
     * @return True iff the set of attachments corresponds to this Family.
     */
    bool operator==(std::span<const u64> ids) const;

    /**
     * @brief Compare two Families.
     *
     * @param other The other Family.
     * @return True if the two Families are equal, false otherwise.
     */
    bool operator==(const Family& other) const;

private:
    usize m_size;
    u64* m_ids;
};

struct IGE_API FamilyHash {
    using is_transparent = void;

    usize operator()(std::span<const u64> ids) const noexcept;
    usize operator()(const Family& family) const noexcept;
};

/**
 * @brief An Archetype groups entities having the same attachments.
 *
 * The Archetype does not store component data directly. Rather, it stores a set
 * of attachments and an optional pointer to the Table holding the actual data.
 * Since some attachments may not need data, the pointer to the Table may be
 * nullptr.
 */
class IGE_API Archetype {
public:
    Archetype(std::span<const u64> ids);
    ~Archetype();

    inline const Family& family() const noexcept { return m_family; }

    inline u32 entity_count() const noexcept { return m_entity_count; }

    inline u32& entity_count() noexcept { return m_entity_count; }

    inline const Table* table() const noexcept { return m_table; }

    inline Table* table() noexcept { return m_table; }

private:
    Family m_family;
    u32 m_entity_count = 0;
    Table* m_table = nullptr;
};

/**
 * @brief A Record stores information about an Entity.
 *
 * All living entities are associated with a Record, which stores information
 * such as the Entity's archetype and its row in the Table storing the Entity's
 * components.
 */
struct Record {
    Archetype* archetype = nullptr;
    u32 row = 0;
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
     * @param entity The entity to attach to.
     * @param id The attachment to add.
     */
    void attach(Entity entity, u64 id);

    /**
     * @brief Attach an entity to another one.
     *
     * Equivalent to `attach(entity, other.id())`.
     *
     * @param entity The entity to attach to.
     * @param other The entity to attach.
     */
    inline void attach(Entity entity, Entity other)
    {
        attach(entity, other.id());
    }

    /**
     * @brief Remove an attachment from an entity.
     *
     * @param entity The entity to detach from.
     * @param id The attachment to remove.
     */
    void detach(Entity entity, u64 id);

    /**
     * @brief Detach an entity from another one.
     *
     * Equivalent to `detach(entity, other.id())`.
     *
     * @param entity The entity to detach from.
     * @param other The entity to detach.
     */
    inline void detach(Entity entity, Entity other)
    {
        detach(entity, other.id());
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

    template <Component C>
    const C* get() const;

private:
    using EntityList = std::vector<Entity>;
    using EntityIndex = std::unordered_map<u64, Record>;
    using ArchetypeIndex
        = std::unordered_map<Family, Archetype, FamilyHash, std::equal_to<>>;

    Archetype* get_archetype(std::span<const u64>);

    u32 m_tick = 0;
    u32 m_last_entity_id = 0;
    EntityList* m_free_entities = nullptr;
    EntityIndex* m_entity_index = nullptr;
    ArchetypeIndex* m_archetypes = nullptr;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
