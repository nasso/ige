/*
** EPITECH PROJECT, 2021
** un
** File description:
** System
*/

#include "un/ecs/System.hpp"
#include "un/ecs/World.hpp"

namespace un {
namespace ecs {

    System::System(std::function<void(World&)> run)
        : m_run(run)
    {
    }

    void System::run(World& wld)
    {
        m_run(wld);
    }

}
}
