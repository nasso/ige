#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Control.hpp"

namespace ige::ecs {

World::World()
    : m_free_entities(new std::vector<Entity>())
    , m_entities(new std::unordered_set<u64>())
{
}

World::~World()
{
    delete m_free_entities;
    delete m_entities;
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

    m_entities->insert(e.idgen());

    return e;
}

void World::destroy(Entity entity)
{
    m_entities->erase(entity.idgen());
    m_free_entities->emplace_back(entity.id(), entity.gen() + 1);
}

bool World::is_alive(Entity entity) const
{
    return m_entities->contains(entity.idgen());
}

void World::attach(Entity, u64) { IGE_TODO(); }

void World::detach(Entity, u64) { IGE_TODO(); }

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

}
