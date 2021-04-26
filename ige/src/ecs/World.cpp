/*
** EPITECH PROJECT, 2021
** ige
** File description:
** World
*/

#include "ige/ecs/World.hpp"

namespace ige {
namespace ecs {

    World::Entity::Entity(World& wld, EntityId id)
        : m_wld(wld)
        , m_id(id)
    {
    }

    bool World::Entity::operator==(const Entity& other) const
    {
        return m_id == other.m_id && &m_wld == &other.m_wld;
    }

    bool World::Entity::operator!=(const Entity& other) const
    {
        return !(*this == other);
    }

    bool World::Entity::remove()
    {
        return m_wld.remove_entity(m_id);
    }

    World::Entity World::create_entity()
    {
        return { *this, ++m_last_entity };
    }

    bool World::remove_entity(EntityId ent)
    {
        for (auto& comp : m_components) {
            comp.second(ent);
        }

        return true;
    }

}
}
