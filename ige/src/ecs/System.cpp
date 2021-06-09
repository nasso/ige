#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <functional>
#include <memory>

using ige::ecs::System;
using ige::ecs::World;

class FunctionSystem : public System {
public:
    FunctionSystem(std::function<void(World&)> run) noexcept
        : m_run(run)
    {
    }

    void run(World& world) override
    {
        m_run(world);
    }

private:
    std::function<void(World&)> m_run;
};

std::unique_ptr<System> System::from(std::function<void(World&)> fn)
{
    return std::make_unique<FunctionSystem>(fn);
}
