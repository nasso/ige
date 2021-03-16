/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#include "un/App.hpp"

namespace un {

void App::run()
{
    for (auto& system : m_startup_systems) {
        system.run(m_world);
    }

    for (auto& system : m_systems) {
        system.run(m_world);
    }
}

}
