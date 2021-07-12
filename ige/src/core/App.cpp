#include "igepch.hpp"

#include "ige/core/App.hpp"
#include "ige/core/StateMachine.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

using ige::core::App;
using ige::core::StateMachine;
using ige::ecs::Resources;
using ige::ecs::Schedule;
using ige::ecs::World;

App::App(
    Resources res, Schedule on_start, Schedule on_update, Schedule on_cleanup)
    : m_world(std::move(res))
    , m_startup(std::move(on_start))
    , m_update(std::move(on_update))
    , m_cleanup(std::move(on_cleanup))
{
}

App::App(Resources res)
    : m_world(std::move(res))
{
}

App::Builder&
App::Builder::add_startup_system(std::unique_ptr<ecs::System> system) &
{
    m_startup.add_system(std::move(system));
    return *this;
}

App::Builder
App::Builder::add_startup_system(std::unique_ptr<ecs::System> system) &&
{
    return std::move(add_startup_system(std::move(system)));
}

App::Builder& App::Builder::add_system(std::unique_ptr<ecs::System> system) &
{
    m_update.add_system(std::move(system));
    return *this;
}

App::Builder App::Builder::add_system(std::unique_ptr<ecs::System> system) &&
{
    return std::move(add_system(std::move(system)));
}

App::Builder&
App::Builder::add_cleanup_system(std::unique_ptr<ecs::System> system) &
{
    m_cleanup.add_system(std::move(system));
    return *this;
}

App::Builder
App::Builder::add_cleanup_system(std::unique_ptr<ecs::System> system) &&
{
    return std::move(add_cleanup_system(std::move(system)));
}

App::Builder& App::Builder::add_plugin(const App::Plugin& plugin)
{
    plugin.plug(*this);
    return *this;
}

World& App::world()
{
    return m_world;
}

const World& App::world() const
{
    return m_world;
}

StateMachine& App::state_machine()
{
    return m_state_machine;
}

const StateMachine& App::state_machine() const
{
    return m_state_machine;
}

void App::run()
{
    m_startup.run_forward(m_world);

    do {
        m_state_machine.update(*this);
        m_update.run_forward(m_world);
    } while (m_state_machine.is_running());

    m_cleanup.run_reverse(m_world);
}

void App::quit()
{
    m_state_machine.quit();
}
