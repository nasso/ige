/*
** EPITECH PROJECT, 2021
** un
** File description:
** System
*/

#include "un/ecs/System.hpp"

namespace un {
namespace ecs {

    System::System(std::function<void()> run)
        : m_run(run)
    {
    }

    void System::run()
    {
        m_run();
    }

}
}
