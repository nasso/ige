#ifndef FFD6A665_CA24_4C05_BB07_2739F63344DE
#define FFD6A665_CA24_4C05_BB07_2739F63344DE

#include <cstddef>
#include <cstdint>
#include <functional>
#include <unordered_set>

namespace ige {
namespace ecs {

    class EntityId {
    private:
        std::size_t m_index;
        std::uint64_t m_generation;

        EntityId(std::size_t index, std::uint64_t gen);

    public:
        EntityId(std::size_t index);

        bool operator==(const EntityId&) const = default;

        std::size_t index() const;
        std::uint64_t generation() const;
        EntityId next_gen() const;
    };

}
}

namespace std {

template <>
struct hash<ige::ecs::EntityId> {
    std::size_t operator()(const ige::ecs::EntityId& entity) const
    {
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<std::size_t>()(entity.index());
        hash = hash * 31 + std::hash<std::uint64_t>()(entity.generation());
        return hash;
    }
};

}

namespace ige {
namespace ecs {

    class EntityPool {
    private:
        std::unordered_set<EntityId> m_entities;
        std::unordered_set<EntityId> m_released;

        std::size_t m_size = 0;

    public:
        /**
         * @brief Create and return a new EntityId.
         *
         * @return EntityId
         */
        EntityId allocate();

        /**
         * @brief Mark an EntityId as deleted.
         *
         * A released EntityId might be returned by a later call to
         * `EntityPool::allocate` under a new generation.
         *
         * @param entity The EntityId to release
         * @return true if `entity` existed and has been released
         * @return false if `entity` didn't exist - no action taken
         */
        bool release(EntityId entity);

        /**
         * @brief Check for the existence of an entity.
         *
         * @param entity
         * @return true
         * @return false
         */
        bool contains(EntityId entity) const;
    };

}
}

#endif /* FFD6A665_CA24_4C05_BB07_2739F63344DE */
