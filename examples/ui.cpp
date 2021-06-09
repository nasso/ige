#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <glm/vec3.hpp>
#include <iostream>
#include <optional>

using glm::vec3;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::World;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputPlugin;
using ige::plugin::render::ImageRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RectRenderer;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Parent;
using ige::plugin::transform::RectTransform;
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
            PerspectiveCamera { 70.0f },
            Scripts::from(TrackballCamera { 10.0f }));

        app.world().create_entity(
            Transform {}.set_scale(vec3 { 5.0f, 0.1f, 5.0f }),
            GltfScene { "assets/test_box.glb", GltfFormat::BINARY });

        app.world().create_entity(
            Transform {},
            GltfScene { "assets/test_box.glb", GltfFormat::BINARY });

        // a rect transform without a parent will act like if its parent was the
        // entire window surface (its anchors will be proportional to the window
        // size)
        auto bottom_pane = app.world().create_entity(
            RectTransform {}
                .set_anchors({ 0.0f, 0.0f }, { 1.0f, 0.0f })
                .set_bounds({ 0.0f, 0.0f }, { 0.0f, 50.0f }),
            RectRenderer {}.set_fill_rgb(0xFFFFFF));

        auto btn_img = Texture::make_new("assets/button_frame.png");

        app.world().create_entity(
            Parent { bottom_pane },
            RectTransform {}
                .set_anchors({ 0.0f, 0.5f })
                .set_bounds({ 5.0f, -20.0f }, { 100.0f, 20.0f }),
            ImageRenderer { btn_img, ImageRenderer::Mode::SLICED }
                // apply a yellowish tint
                .set_tint_rgb(0xfce37e));

        app.world().create_entity(
            RectTransform {}
                .set_anchors({ 0.0f, 1.0f }, { 1.0f, 1.0f })
                .set_bounds({ 100.0f, -100.0f }, { -100.0f, -10.0f }),
            ImageRenderer { btn_img, ImageRenderer::Mode::TILED });
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
