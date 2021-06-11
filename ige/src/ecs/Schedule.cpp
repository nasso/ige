#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <memory>
#include <ranges>
#include <vector>

using ige::ecs::Schedule;
using ige::ecs::System;

Schedule::Schedule(std::vector<std::unique_ptr<System>> sys)
    : m_systems(std::move(sys))
{
}

Schedule::Builder&
Schedule::Builder::add_system(std::unique_ptr<System> system) &
{
    m_systems.push_back(std::move(system));
    return *this;
}

Schedule::Builder
Schedule::Builder::add_system(std::unique_ptr<System> system) &&
{
    return std::move(add_system(std::move(system)));
}

Schedule Schedule::Builder::build()
{
    return Schedule(std::move(m_systems));
}

void Schedule::run_forward(World& world)
{
    for (auto& sys : m_systems) {
        sys->run(world);
    }
}

void Schedule::run_reverse(World& world)
{
    for (auto& sys : std::views::reverse(m_systems)) {
        sys->run(world);
    }
}
