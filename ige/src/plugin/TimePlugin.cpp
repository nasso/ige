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
using std::chrono::steady_clock;

Time::Time()
    : m_start_time(steady_clock::now())
    , m_last_update(m_start_time)
    , m_delta(0)
{
}

std::chrono::duration<float> Time::since_startup() const
{
    return m_last_update - m_start_time;
}

std::chrono::duration<float> Time::delta() const
{
    return m_delta;
}

float Time::seconds_since_startup() const
{
    return since_startup().count();
}

float Time::delta_seconds() const
{
    return delta().count();
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
