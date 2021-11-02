#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "igepch.hpp"

namespace ige::ecs {

void Archetype::merge_from(const Archetype& other)
{
    m_entity_count += other.m_entity_count;
    // TODO: merge tables
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
    // detach from all entities that had this entity as a component
    auto it = m_archetypes->begin();

    while (it != m_archetypes->end()) {
        auto& [fam, arch] = *it;

        // this family of entity has this entity as a component
        if (fam.has(entity.id())) {
            // lookup the family that does not have it
            auto tit = m_archetypes->find(fam.without(entity.id()));

            if (tit != m_archetypes->end()) {
                // we found a family that is the same as fam without entity
                // just move all entities there
                tit->second.merge_from(arch);
                m_archetypes->erase(it++);
            } else {
                // it doesn't exist, so we have to create it
                // since fam won't exist anymore, we can safely replace it
                auto nh = m_archetypes->extract(it++);

                // todo: mutate the family instead of creating a new one
                nh.key() = Family(fam.without(entity.id()));
                m_archetypes->insert(std::move(nh));
            }
        } else {
            ++it;
        }
    }

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

    record.type = get_archetype(record.type.family->with(id));
}

void World::detach(Entity entity, u64 id)
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    auto& record = m_entity_index->at(entity.idgen());

    record.type = get_archetype(record.type.family->without(id));
}

bool World::has(Entity entity, u64 id) const
{
    IGE_ASSERT(is_alive(entity), "Entity is not alive");

    return m_entity_index->at(entity.idgen()).type.family->has(id);
}

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

ArchetypeRecord World::empty_archetype()
{
    return get_archetype(std::span<const u64>());
}

ArchetypeRecord World::get_archetype(std::span<const u64> ids)
{
    auto it = m_archetypes->find(ids);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(ids, Archetype()).first;
    }

    return { &it->first, &it->second };
}

ArchetypeRecord World::get_archetype(Family::With w)
{
    auto it = m_archetypes->find(w);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(w, Archetype()).first;
    }

    return { &it->first, &it->second };
}

ArchetypeRecord World::get_archetype(Family::Without w)
{
    auto it = m_archetypes->find(w);

    if (it == m_archetypes->end()) {
        it = m_archetypes->emplace(w, Archetype()).first;
    }

    return { &it->first, &it->second };
}

}
