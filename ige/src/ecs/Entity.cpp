#include "ige/ecs/Entity.hpp"
#include <cstddef>
#include <cstdint>
#include <unordered_set>

using ige::ecs::EntityId;
using ige::ecs::EntityPool;

EntityId::EntityId(std::size_t index, std::uint64_t gen)
    : m_index(index)
    , m_generation(gen)
{
}

std::size_t EntityId::index() const
{
    return m_index;
}

std::uint64_t EntityId::generation() const
{
    return m_generation;
}

EntityId EntityId::next_gen() const
{
    return { m_index, m_generation + 1 };
}

EntityId EntityPool::allocate()
{
    auto it_available = m_released.begin();

    if (it_available != m_released.end()) {
        EntityId entity = it_available->next_gen();
        m_released.erase(it_available);
        m_entities.insert(entity);
        return entity;
    } else {
        EntityId entity { m_size++, 0 };
        m_entities.insert(entity);
        return entity;
    }
}

bool EntityPool::release(EntityId entity)
{
    auto iter = m_entities.find(entity);

    if (iter != m_entities.end()) {
        m_released.insert(entity);
        m_entities.erase(iter);
        return true;
    }

    return false;
}

bool EntityPool::exists(EntityId entity) const
{
    return m_entities.contains(entity);
}
