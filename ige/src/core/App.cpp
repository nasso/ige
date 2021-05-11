#include "ige/core/App.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include <optional>

namespace ige {
namespace core {

    App::App(ecs::Resources res, ecs::Schedule on_start,
        ecs::Schedule on_update, ecs::Schedule on_cleanup)
        : m_world(std::move(res))
        , m_startup(std::move(on_start))
        , m_update(std::move(on_update))
        , m_cleanup(std::move(on_cleanup))
    {
    }

    App::App(ecs::Resources res)
        : m_world(std::move(res))
    {
    }

    App::Builder& App::Builder::add_plugin(const App::Plugin& plugin)
    {
        plugin.plug(*this);
        return *this;
    }

    App::Builder& App::Builder::add_startup_system(ecs::System sys)
    {
        m_startup.add_system(sys);
        return *this;
    }

    App::Builder& App::Builder::add_system(ecs::System sys)
    {
        m_update.add_system(sys);
        return *this;
    }

    App::Builder& App::Builder::add_cleanup_system(ecs::System sys)
    {
        m_cleanup.add_system(sys);
        return *this;
    }

    ecs::World& App::world()
    {
        return m_world;
    }

    const ecs::World& App::world() const
    {
        return m_world;
    }

    core::StateMachine& App::state_machine()
    {
        return m_state_machine;
    }

    const core::StateMachine& App::state_machine() const
    {
        return m_state_machine;
    }

    void App::run()
    {
        m_startup(m_world);

        while (m_state_machine.is_running()) {
            m_state_machine.update(*this);
            m_update(m_world);
        }

        m_cleanup(m_world);
    }

    void App::quit()
    {
        m_state_machine.quit();
    }

}
}
