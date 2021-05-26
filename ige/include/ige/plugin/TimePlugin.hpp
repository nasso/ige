#ifndef F86E20B8_2AFA_4597_AD7A_707E4AEA22BC
#define F86E20B8_2AFA_4597_AD7A_707E4AEA22BC

#include "ige/core/App.hpp"
#include <chrono>

namespace ige::plugin::time {

class Time {
private:
    std::chrono::time_point<std::chrono::steady_clock> m_start_time;
    std::chrono::time_point<std::chrono::steady_clock> m_last_update;
    std::chrono::duration<float> m_delta;

public:
    Time();

    void update();

    std::chrono::duration<float> since_startup() const;
    std::chrono::duration<float> delta() const;

    float seconds_since_startup() const;
    float delta_seconds() const;
};

class TimePlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* F86E20B8_2AFA_4597_AD7A_707E4AEA22BC */
