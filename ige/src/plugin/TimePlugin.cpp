#include "ige/plugin/TimePlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <chrono>

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
    , m_delta(0)
{
}

void Time::set_tick_duration(steady_clock::duration tick)
{
    m_tick = tick;
}

steady_clock::duration Time::now() const
{
    return m_last_update - m_start_time;
}

steady_clock::duration Time::delta() const
{
    return m_delta;
}

steady_clock::duration Time::tick() const
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

void Time::update()
{
    auto now = steady_clock::now();
    m_delta = now - m_last_update;
    m_last_update = now;
}

namespace systems {

static void update_global_time(World& world)
{
    world.get_or_emplace<Time>().update();
}

}

void TimePlugin::plug(App::Builder& builder) const
{
    builder.emplace<Time>();
    builder.add_system(System(systems::update_global_time));
}
