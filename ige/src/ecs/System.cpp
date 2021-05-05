#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

namespace ige {
namespace ecs {

    System::System(std::function<void(World&)> run) noexcept
        : m_run(run)
    {
    }

    void System::operator()(World& wld)
    {
        m_run(wld);
    }

}
}
