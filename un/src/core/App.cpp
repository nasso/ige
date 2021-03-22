/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#include "un/core/App.hpp"
#include "rtl/Option.hpp"

using rtl::some;

namespace un {
namespace core {

    void App::Builder::on_start(ecs::Schedule sch)
    {
        m_startup.replace(sch);
    }

    void App::Builder::on_update(ecs::Schedule sch)
    {
        m_update.replace(sch);
    }

    void App::Builder::on_stop(ecs::Schedule sch)
    {
        m_cleanup.replace(sch);
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
