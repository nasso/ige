#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <ranges>

namespace ige {
namespace ecs {

    Schedule::Schedule(std::vector<System> sys)
        : m_systems(sys)
    {
    }

    Schedule Schedule::Builder::build() const
    {
        return { m_systems };
    }

    void Schedule::run_forward(World& world)
    {
        for (auto& sys : m_systems) {
            sys(world);
        }
    }

    void Schedule::run_reverse(World& world)
    {
        for (auto& sys : std::views::reverse(m_systems)) {
            sys(world);
        }
    }

}
}
