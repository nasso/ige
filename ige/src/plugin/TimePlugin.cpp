#include "igepch.hpp"

#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/TimePlugin.hpp"
#include <chrono>
#include <cstdint>
#include <ratio>

using ige::core::App;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using std::chrono::duration;
using std::chrono::duration_cast;
using std::chrono::steady_clock;

Time::Time()
    : m_start_time(steady_clock::now())
    , m_last_update(m_start_time)
    , m_last_tick(m_start_time)
{
}

void Time::set_tick_duration(Time::Duration tick)
{
    m_tick = tick;
}

Time::Duration Time::now() const
{
    return m_last_update - m_start_time;
}

Time::Duration Time::delta() const
{
    return m_delta;
}

Time::Duration Time::tick() const
{
    return m_tick;
}

float Time::now_seconds() const
{
    return duration_cast<duration<float>>(now()).count();
}

float Time::delta_seconds() const
{
    return duration_cast<duration<float>>(delta()).count();
}

float Time::tick_seconds() const
{
    return duration_cast<duration<float>>(tick()).count();
}

std::uint32_t Time::ticks() const
{
    return static_cast<std::uint32_t>((m_last_update - m_last_tick) / m_tick);
}

void Time::update()
{
    m_last_tick += m_tick * ticks();

    auto now = steady_clock::now();
    m_delta = now - m_last_update;
    m_last_update = now;
}

namespace systems {

struct FrameStats {
    Time::Duration last_log;
    std::uint64_t frames = 0;

    FrameStats(const Time& time)
        : last_log(time.now())
    {
    }
};

static void update_global_time(World& world)
{
    auto& time = world.get_or_emplace<Time>();
    time.update();

    auto& stats = world.get_or_emplace<FrameStats>(time);
    auto time_since_last_log = time.now() - stats.last_log;

    if (time_since_last_log >= std::chrono::seconds(1)) {
        auto average = time_since_last_log / stats.frames;
        auto average_ms = duration_cast<duration<float, std::milli>>(average);
        float fps = stats.frames
            / duration_cast<duration<float>>(time_since_last_log).count();

        std::cout << "[INFO] Average frame time: " << average_ms.count() << "ms"
                  << " (" << fps << " FPS)" << std::endl;
        stats.last_log = time.now();
        stats.frames = 0;
    }

    stats.frames++;
}

}

void TimePlugin::plug(App::Builder& builder) const
{
    builder.emplace<Time>();
    builder.add_system(System::from(systems::update_global_time));
}
