
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

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

    void Schedule::operator()(World& world)
    {
        for (auto& sys : m_systems) {
            sys(world);
        }
    }

}
}
