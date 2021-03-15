/*
** EPITECH PROJECT, 2021
** un
** File description:
** World
*/

#include "un/ecs/World.hpp"

namespace un {
namespace ecs {

    World::EntityId World::spawn()
    {
        return ++m_last_entity;
    }

}
}
