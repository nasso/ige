#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <glm/vec3.hpp>
#include <iostream>
#include <optional>

using glm::vec3;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::World;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputPlugin;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        app.world().create_entity(
            Transform {}.set_scale(vec3 { 5.0f, 0.1f, 5.0f }),
            GltfScene { "assets/test_box.glb", GltfFormat::BINARY });

        app.world().create_entity(
            Transform {},
            GltfScene { "assets/test_box.glb", GltfFormat::BINARY });

        app.world().create_entity(
            PerspectiveCamera { 70.0f },
            Scripts::from(TrackballCamera { 10.0f }));
    }

    void on_update(App& app) override
    {
        while (const auto& event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
    }
};

int main()
{
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(TimePlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(InputPlugin {})
        .add_plugin(GltfPlugin {})
        .add_plugin(ScriptPlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(RenderPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
