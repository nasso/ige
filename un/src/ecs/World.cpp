/*
** EPITECH PROJECT, 2021
** un
** File description:
** World
*/

#include "un/ecs/World.hpp"

namespace un {
namespace ecs {

    EntityId World::create_entity()
    {
        return ++m_last_entity;
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
