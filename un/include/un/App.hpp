/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "ecs/System.hpp"
#include "ecs/World.hpp"
#include <vector>

namespace un {

class App {
public:
    App() = default;

    void run();

    template <typename T>
    void add_startup_system(T system)
    {
        m_startup_systems.emplace_back(system);
    }

    template <typename T>
    void add_system(T system)
    {
        m_systems.emplace_back(system);
    }

private:
    ecs::World m_world;
    std::vector<ecs::System> m_startup_systems;
    std::vector<ecs::System> m_systems;
};

}

#endif /* F7EA1363_932A_4910_8D9B_4CA25912EBED */
