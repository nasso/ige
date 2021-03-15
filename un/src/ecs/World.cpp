/*
** EPITECH PROJECT, 2021
** un
** File description:
** World
*/

#include "un/ecs/World.hpp"

namespace un {
namespace ecs {

    World::EntityId World::createEntity()
    {
        return ++m_last_entity;
    }

    bool World::removeEntity(World::EntityId ent)
    {
        for (auto& comp : m_components) {
            comp.second(ent);
        }

        return true;
    }

}
}
