#ifndef F86E20B8_2AFA_4597_AD7A_707E4AEA22BC
#define F86E20B8_2AFA_4597_AD7A_707E4AEA22BC

#include "ige/core/App.hpp"
#include <chrono>

namespace ige::plugin::time {

class Time {
private:
    std::chrono::steady_clock::time_point m_start_time;
    std::chrono::steady_clock::time_point m_last_update;
    std::chrono::steady_clock::duration m_delta;
    std::chrono::steady_clock::duration m_tick
        = std::chrono::round<std::chrono::steady_clock::duration>(
            std::chrono::duration<float>(1.0f / 50.0f));

public:
    Time();

    void update();
    void set_tick_duration(std::chrono::steady_clock::duration);

    std::chrono::steady_clock::duration now() const;
    std::chrono::steady_clock::duration delta() const;
    std::chrono::steady_clock::duration tick() const;
    float now_seconds() const;
    float delta_seconds() const;
};

class TimePlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* F86E20B8_2AFA_4597_AD7A_707E4AEA22BC */
