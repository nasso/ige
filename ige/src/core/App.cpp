
#include "ige/core/App.hpp"
#include <optional>

namespace ige {
namespace core {

    App::Builder& App::Builder::on_start(ecs::Schedule sch)
    {
        m_startup = sch;
        return *this;
    }

    App::Builder& App::Builder::on_update(ecs::Schedule sch)
    {
        m_update = sch;
        return *this;
    }

    App::Builder& App::Builder::on_stop(ecs::Schedule sch)
    {
        m_cleanup = sch;
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
        if (m_startup) {
            (*m_startup)(m_world);
        }

        while (m_state_machine.is_running()) {
            m_state_machine.update(*this);
            if (m_update) {
                (*m_update)(m_world);
            }
        }

        if (m_cleanup) {
            (*m_cleanup)(m_world);
        }
    }

    void App::quit()
    {
        m_state_machine.quit();
    }

}
}
