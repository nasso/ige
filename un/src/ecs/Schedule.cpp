/*
** EPITECH PROJECT, 2021
** un
** File description:
** Schedule
*/

#include "un/ecs/Schedule.hpp"
#include "un/ecs/System.hpp"
#include "un/ecs/World.hpp"

namespace un {
namespace ecs {

    Schedule::Schedule(std::vector<System> sys)
        : m_systems(sys)
    {
    }

    Schedule::Builder& Schedule::Builder::add_system(System sys)
    {
        m_systems.push_back(sys);

        return *this;
    }

    Schedule Schedule::Builder::build() const
    {
        return { m_systems };
    }

    void Schedule::operator()(World& world)
    {
        for (auto& sys : m_systems) {
            sys(world);
        }
    }

}
}
