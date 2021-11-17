#ifndef F86E20B8_2AFA_4597_AD7A_707E4AEA22BC
#define F86E20B8_2AFA_4597_AD7A_707E4AEA22BC

#include "ige/core/App.hpp"
#include <chrono>
#include <cstdint>

namespace ige::plugin::time {

class Time {
public:
    using Duration = std::chrono::steady_clock::duration;

    Time();

    void update();
    void set_tick_duration(Duration);

    /**
     * @brief The amount of time elapsed since the application started.
     */
    Duration now() const;

    /**
     * @brief The time elapsed since the last update.
     */
    Duration delta() const;

    /**
     * @brief The duration of a single tick.
     */
    Duration tick() const;

    /**
     * @brief The amount of time elapsed since the application started, in
     * seconds.
     */
    float now_seconds() const;

    /**
     * @brief The time elapsed since the last update, in seconds.
     */
    float delta_seconds() const;

    /**
     * @brief The duration of a single tick, in seconds.
     */
    float tick_seconds() const;

    /**
     * @brief How many ticks have passed since the last update.
     */
    std::uint32_t ticks() const;

private:
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::steady_clock::time_point m_last_update;
    std::chrono::steady_clock::time_point m_last_tick;
    Duration m_delta = Duration::zero();
    Duration m_tick = std::chrono::round<Duration>(
        std::chrono::duration<float>(1.0f / 50.0f));
};

class TimePlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* F86E20B8_2AFA_4597_AD7A_707E4AEA22BC */
