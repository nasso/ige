/*
** EPITECH PROJECT, 2021
** oge
** File description:
** App
*/

#ifndef APP_HPP_
#define APP_HPP_

#include "ecs/System.hpp"
#include <vector>

namespace oge {

class App {
public:
    App();

    template <typename T>
    void add_system(T system)
    {
        m_systems.emplace_back(system);
    }

    void run();

private:
    std::vector<ecs::System> m_systems;
};

}

#endif /* !APP_HPP_ */
