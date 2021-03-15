/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#include "un/App.hpp"

namespace un {

App::App()
{
}

void App::run()
{
    for (auto& system : m_systems) {
        system.run();
    }
}

}
