#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include <algorithm>

namespace ige::ecs {

Family::Family(std::span<const u64> ids)
    : m_size(ids.size())
    , m_ids(new u64[m_size])
{
    std::copy(ids.begin(), ids.end(), m_ids);
    std::sort(m_ids, m_ids + m_size);
}

Family::Family(Family&& f)
    : m_size(f.m_size)
    , m_ids(f.m_ids)
{
    f.m_size = 0;
    f.m_ids = nullptr;
}

Family Family::clone() const { return Family({ m_ids, m_size }); }

Family& Family::operator=(Family&& f)
{
    m_size = f.m_size;
    m_ids = f.m_ids;
    f.m_size = 0;
    f.m_ids = nullptr;
    return *this;
}

Family::~Family() { delete[] m_ids; }

usize FamilyHash::operator()(std::span<const u64> ids) const noexcept
{
    usize hash = 17;

    for (auto id : ids) {
        hash = hash * 31 + std::hash<u64>()(id);
    }

    return hash;
}

usize FamilyHash::operator()(const Family& family) const noexcept
{
    return operator()(family.ids());
}

std::span<const u64> Family::ids() const { return { m_ids, m_size }; }

usize Family::size() const { return m_size; }

bool Family::operator==(std::span<const u64> ids) const
{
    if (m_size != ids.size()) {
        return false;
    }

    // ids are sorted at construction, so we can use std::equal
    return std::equal(ids.begin(), ids.end(), m_ids);
}

bool Family::operator==(const Family& other) const
{
    return *this == std::span<const u64>(other.m_ids, other.m_size);
}

Archetype::Archetype(std::span<const u64> ids)
    : m_family(ids)
    , m_entity_count(0)
    , m_table(nullptr)
{
}

Archetype::~Archetype() { delete m_table; }

World::World()
    : m_free_entities(new EntityList)
    , m_entity_index(new EntityIndex)
    , m_archetypes(new ArchetypeIndex)
{
}

World::~World()
{
    delete m_free_entities;
    delete m_entity_index;
    delete m_archetypes;
}

Entity World::entity()
{
    const bool recycle = !m_free_entities->empty();
    Entity e = recycle ? m_free_entities->back() : Entity(m_last_entity_id, 0);

    if (recycle) {
        m_free_entities->pop_back();
    } else {
        m_last_entity_id++;
    }

    m_entity_index->emplace(e.idgen(), Record());

    return e;
}

void World::destroy(Entity entity)
{
    m_entity_index->erase(entity.idgen());
    m_free_entities->emplace_back(entity.id(), entity.gen() + 1);
}

bool World::is_alive(Entity entity) const
{
    return m_entity_index->contains(entity.idgen());
}

void World::attach(Entity entity, u64 id)
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    Record& record = m_entity_index->at(entity.idgen());

    if (!record.archetype) {
        record.archetype = get_archetype({ &id, 1 });
    } else {
        IGE_TODO();
    }
}

void World::detach(Entity entity, u64 id) { IGE_TODO(); }

bool World::has(Entity entity, u64 id) const
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    const Record& record = m_entity_index->at(entity.idgen());

    if (!record.archetype) {
        return false;
    }

    const auto& ids = record.archetype->family().ids();

    // we can use std::binary_search because ids are sorted
    return std::binary_search(ids.begin(), ids.end(), id);
}

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

Archetype* World::get_archetype(std::span<const u64> ids)
{
    auto it = m_archetypes->find(ids);
    if (it != m_archetypes->end()) {
        return &it->second;
    }

    return &m_archetypes->emplace(ids, ids).first->second;
}

}
