#include "ige/ecs/World.hpp"
#include "ige/ecs/Resources.hpp"

using ige::ecs::EntityId;
using ige::ecs::Resources;
using ige::ecs::World;

World::World(Resources res)
    : m_resources(std::move(res))
{
}

bool World::exists(const EntityId& ent)
{
    return m_entities.exists(ent);
}

bool World::remove_entity(const EntityId& ent)
{
    if (m_entities.release(ent)) {
        m_generation.remove(ent.index());

        for (auto& comp : m_components) {
            comp.second(ent);
        }

        return true;
    }

    return false;
}
