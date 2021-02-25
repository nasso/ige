/*
** EPITECH PROJECT, 2021
** oge
** File description:
** System
*/

#include "oge/ecs/System.hpp"

namespace oge {
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
