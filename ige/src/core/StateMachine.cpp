/*
** EPITECH PROJECT, 2021
** ige
** File description:
** StateMachine
*/

#include "ige/core/StateMachine.hpp"
#include "ige/core/App.hpp"
#include <functional>
#include <optional>

namespace ige {
namespace core {

    void StateMachine::remove_current_state()
    {
        if (!m_states.empty()) {
            m_stopping.insert(m_stopping.end(),
                std::make_move_iterator(m_states.end() - 1),
                std::make_move_iterator(m_states.end()));
            m_states.pop_back();
        }
    }

    void StateMachine::pop()
    {
        if (auto cur = current()) {
            cur->get().stop();
        }

        remove_current_state();

        if (auto cur = current()) {
            cur->get().resume();
        }
    }

    void StateMachine::update(App& app)
    {
        for (auto& state : m_stopping) {
            state->update(app);
        }
        m_stopping.clear();

        for (auto& state : m_states) {
            state->update(app);
        }
    }

    void StateMachine::quit()
    {
        m_states.clear();
    }

    bool StateMachine::is_running() const
    {
        return !m_states.empty();
    }

    std::optional<std::reference_wrapper<State>> StateMachine::current()
    {
        if (m_states.empty()) {
            return {};
        } else {
            return { **(m_states.end() - 1) };
        }
    }

    std::optional<std::reference_wrapper<const State>>
    StateMachine::current() const
    {
        if (m_states.empty()) {
            return {};
        } else {
            return { **(m_states.end() - 1) };
        }
    }

}
}
