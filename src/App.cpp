/*
** EPITECH PROJECT, 2021
** oge
** File description:
** App
*/

#include "oge/App.hpp"

namespace oge {

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
