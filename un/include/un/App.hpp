/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "ecs/System.hpp"
#include <vector>

namespace un {

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

#endif /* F7EA1363_932A_4910_8D9B_4CA25912EBED */
