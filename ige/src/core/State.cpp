
#include "ige/core/State.hpp"

namespace ige {
namespace core {

    State::Status State::status() const
    {
        return m_status;
    }

    void State::start()
    {
        if (m_next_status == STOPPED) {
            m_next_status = RUNNING;
        }
    }

    void State::stop()
    {
        m_next_status = STOPPED;
    }

    void State::pause()
    {
        if (m_next_status == RUNNING) {
            m_next_status = PAUSED;
        }
    }

    void State::resume()
    {
        if (m_next_status == PAUSED) {
            m_next_status = RUNNING;
        }
    }

    void State::update(App& app)
    {
        if (m_status != m_next_status) {
            if (m_status == STOPPED) {
                on_start(app);
            } else if (m_next_status == STOPPED) {
                on_stop(app);
            } else if (m_next_status == RUNNING) {
                on_resume(app);
            }

            if (m_next_status == PAUSED) {
                on_pause(app);
            }

            m_status = m_next_status;
        }

        if (m_status != STOPPED) {
            on_update(app);
        }
    }

    void State::on_start(App&)
    {
    }
    void State::on_update(App&)
    {
    }
    void State::on_pause(App&)
    {
    }
    void State::on_resume(App&)
    {
    }
    void State::on_stop(App&)
    {
    }

}
}
