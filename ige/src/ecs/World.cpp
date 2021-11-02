#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "igepch.hpp"

namespace ige::ecs {

bool Archetype::Without::empty() const noexcept
{
    const auto ids = base.ids();

    return ids.empty() || (ids.size() == 1 && ids.front() == missing_id);
}

usize Archetype::Hash::operator()(std::span<const u64> ids) const noexcept
{
    usize hash = 17;

    for (auto id : ids) {
        hash = hash * 31 + std::hash<u64>()(id);
    }

    return hash;
}

usize Archetype::Hash::operator()(const With& w) const noexcept
{
    usize hash = 17;

    for (auto id : w.base.ids()) {
        if (id > w.extra_id) {
            hash = hash * 31 + std::hash<u64>()(w.extra_id);
        }

        hash = hash * 31 + std::hash<u64>()(id);
    }

    return hash;
}

usize Archetype::Hash::operator()(const Without& w) const noexcept
{
    usize hash = 17;

    for (auto id : w.base.ids()) {
        if (id != w.missing_id) {
            hash = hash * 31 + std::hash<u64>()(id);
        }
    }

    return hash;
}

usize Archetype::Hash::operator()(const Archetype& a) const noexcept
{
    return operator()(a.ids());
}

Archetype::Archetype(std::span<const u64> ids)
    : m_ids(ids.begin(), ids.end())
{
    std::sort(m_ids.begin(), m_ids.end());
}

Archetype::Archetype(With w)
{
    std::span extra(&w.extra_id, 1);

    std::set_union(
        w.base.ids().begin(),
        w.base.ids().end(),
        extra.begin(),
        extra.end(),
        std::back_inserter(m_ids));
}

Archetype::Archetype(Without w)
{
    std::copy_if(
        w.base.ids().begin(),
        w.base.ids().end(),
        std::back_inserter(m_ids),
        [w](u64 id) { return id != w.missing_id; });
}

Archetype::Archetype(Archetype&& a)
    : m_ids(std::move(a.m_ids))
{
}

Archetype& Archetype::operator=(Archetype&& f)
{
    m_ids = std::move(f.m_ids);
    return *this;
}

bool Archetype::operator==(std::span<const u64> ids) const
{
    // ids are sorted at construction, so we can use std::equal
    return std::equal(ids.begin(), ids.end(), m_ids.begin(), m_ids.end());
}

bool Archetype::operator==(const Archetype& other) const
{
    return m_ids == other.m_ids;
}

bool Archetype::operator==(const With& w) const
{
    auto w_ids = w.base.ids();

    // if m_ids is empty or smaller than w_ids, no match is possible
    if (m_ids.empty() || m_ids.size() < w_ids.size()) {
        return false;
    }

    // handle the case where w_ids is empty
    // m_ids must be [w.extra_id]
    if (w_ids.empty()) {
        return m_ids.size() == 1 && m_ids[0] == w.extra_id;
    }

    // now we know m_ids and w_ids are both sorted and non-empty

    // compare m_ids to desc_ids where:
    // l = all x in w_ids where x < w.extra_id
    // g = all x in w_ids where x > w.extra_id
    // desc_ids = [l... w.extra_id g...]

    for (usize i = 0; i < m_ids.size(); i++) {
        u64 id = m_ids[i];

        if (id == w.extra_id) {
            // we found the extra id, so we can compare the rest of the ids
            // (the g part of the ids)
            return std::equal(
                w_ids.begin() + i,
                w_ids.end(),
                m_ids.begin() + i);
        } else if (id > w.extra_id || id != w_ids[i]) {
            // we never found the extra id
            // id < w.extra_id, and we found a mismatch
            break;
        }
    }

    return false;
}

bool Archetype::operator==(const Without& w) const
{
    auto w_ids = w.base.ids();

    auto x = std::lower_bound(w_ids.begin(), w_ids.end(), w.missing_id);

    if (x == w_ids.end()) {
        // w.missing_id is not in w_ids
        return std::equal(
            m_ids.begin(),
            m_ids.end(),
            w_ids.begin(),
            w_ids.end());
    }

    return std::equal(w_ids.begin(), x, m_ids.begin())
        && std::equal(
               x + 1,
               w_ids.end(),
               m_ids.begin() + (w_ids.end() - x),
               m_ids.end());
}

auto Archetype::with(u64 id) const -> With { return { *this, id }; }

auto Archetype::without(u64 id) const -> Without { return { *this, id }; }

bool Archetype::has(u64 id) const noexcept
{
    // we can use std::binary_search because ids are sorted
    return std::binary_search(m_ids.begin(), m_ids.end(), id);
}

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

    m_entity_index->emplace(e.idgen(), Record { empty_archetype(), 0 });

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

    record.archetype = get_archetype(record.archetype.get().with(id));
}

void World::detach(Entity entity, u64 id)
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    auto& record = m_entity_index->at(entity.idgen());

    record.archetype = get_archetype(record.archetype.get().without(id));
}

bool World::has(Entity entity, u64 id) const
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    return m_entity_index->at(entity.idgen()).archetype.get().has(id);
}

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

const Archetype& World::empty_archetype()
{
    return get_archetype(std::span<const u64>());
}

const Archetype& World::get_archetype(std::span<const u64> ids)
{
    auto it = m_archetypes->find(ids);

    if (it != m_archetypes->end()) {
        return *it;
    }

    return *m_archetypes->emplace(ids).first;
}

const Archetype& World::get_archetype(Archetype::With w)
{
    auto it = m_archetypes->find(w);

    if (it != m_archetypes->end()) {
        return *it;
    }

    return *m_archetypes->emplace(w).first;
}

const Archetype& World::get_archetype(Archetype::Without w)
{
    auto it = m_archetypes->find(w);

    if (it != m_archetypes->end()) {
        return *it;
    }

    return *m_archetypes->emplace(w).first;
}

}
