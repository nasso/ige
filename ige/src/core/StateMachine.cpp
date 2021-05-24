#include "ige/core/StateMachine.hpp"
#include "ige/core/App.hpp"
#include "ige/core/State.hpp"
#include <functional>
#include <optional>

using ige::core::App;
using ige::core::State;
using ige::core::StateMachine;

void StateMachine::perform(StateMachine::Command cmd, App& app)
{
    switch (cmd.type) {
    case StateMachine::CommandType::POP:
        if (!m_states.empty()) {
            m_states.back()->on_stop(app);
            m_states.pop_back();
        }

        if (!m_states.empty()) {
            m_states.back()->on_resume(app);
        }
        break;
    case StateMachine::CommandType::PUSH:
        if (!m_states.empty()) {
            m_states.back()->on_pause(app);
        }

        m_states.push_back(std::move(cmd.state));
        m_states.back()->on_start(app);
        break;
    case StateMachine::CommandType::QUIT:
        for (auto& state : m_states) {
            state->on_stop(app);
        }

        m_states.clear();
        break;
    case StateMachine::CommandType::SWITCH:
        if (!m_states.empty()) {
            m_states.back()->on_stop(app);
            m_states.pop_back();
        }

        m_states.push_back(std::move(cmd.state));
        m_states.back()->on_start(app);
        break;
    }
}

void StateMachine::queue(StateMachine::Command cmd)
{
    m_command_queue.push(std::move(cmd));
}

void StateMachine::pop()
{
    queue({ CommandType::POP, nullptr });
}

void StateMachine::quit()
{
    queue({ CommandType::QUIT, nullptr });
}

void StateMachine::update(App& app)
{
    while (!m_command_queue.empty()) {
        perform(std::move(m_command_queue.front()), app);
        m_command_queue.pop();
    }

    for (auto& state : m_states) {
        state->on_update(app);
    }
}

bool StateMachine::is_running() const
{
    return !m_states.empty();
}

State* StateMachine::current()
{
    if (m_states.empty()) {
        return nullptr;
    } else {
        return (m_states.end() - 1)->get();
    }
}

const State* StateMachine::current() const
{
    if (m_states.empty()) {
        return nullptr;
    } else {
        return (m_states.end() - 1)->get();
    }
}
