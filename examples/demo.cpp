#include "ige.hpp"
#include <iostream>
#include <optional>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::game::PerspectiveCamera;
using ige::game::RenderingPlugin;
using ige::game::Transform;
using ige::game::WindowEvent;
using ige::game::WindowEventKind;
using ige::game::WindowingPlugin;
using ige::game::WindowSettings;
using ige::math::Vec3;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto camera = app.world().create_entity();

        camera.add_component(Transform::look_at(Vec3(3.0f), Vec3(0.0f)));
        camera.add_component(PerspectiveCamera(90.0f));

        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->get().subscribe());
    }

    void on_update(App& app) override
    {
        while (const auto& event = m_win_events->next_event()) {
            if (event->get().kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
    }
};

int main()
{
    App::Builder()
        .add_plugin(WindowingPlugin {})
        .add_plugin(RenderingPlugin {})
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .run<RootState>();
}
