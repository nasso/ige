#include "TrackballPlugin.hpp"
#include <ige.hpp>
#include <optional>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::World;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputPlugin;
using ige::plugin::render::Light;
using ige::plugin::render::LightType;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class DemoState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription>
        m_window_subscription;

    void on_start(App& app) override
    {
        World& world = app.world();

        world.create_entity(
            Transform {}, GltfScene("assets/Box.glb", GltfFormat::BINARY));
        world.create_entity(
            PerspectiveCamera(90.0F), TrackballController(5.0f));
        world.create_entity(Transform {}, Light::ambient(0.2f));
        world.create_entity(
            Transform {}.look_at({ -1.0f, -2.0f, -3.0f }),
            Light::directional(0.4f));
        world.create_entity(
            Transform {}.look_at({ -1.0f, -2.0f, 3.0f }),
            Light::directional(0.4f));
        world.create_entity(
            Transform {}.look_at({ 3.0f, 2.0f, -1.0f }),
            Light::directional(0.4f));

        m_window_subscription
            = world.get<EventChannel<WindowEvent>>()->subscribe();
    }

    void on_update(App& app) override
    {
        World& world = app.world();

        if (auto window_event = m_window_subscription->next_event()) {
            if (window_event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
    }
};

int main()
{
    App::Builder()
        .add_plugin(TransformPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(InputPlugin {})
        .add_plugin(TimePlugin {})
        .add_plugin(GltfPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(TrackballPlugin {})
        .insert(WindowSettings("Skybox example", 640, 480))
        .run<DemoState>();
}
