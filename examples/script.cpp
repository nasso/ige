#include "ige.hpp"
#include <chrono>
#include <iostream>
#include <optional>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::ecs::World;
using ige::plugin::input::InputPlugin;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
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
        m_win_events.emplace(channel->get().subscribe());

        auto camera = app.world().create_entity(
            Transform::from_pos(vec3(-3.0f, 3.0f, 0.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        app.world().create_entity(
            Transform {},
            MeshRenderer {
                Mesh::make_cube(1.0f),
                Material::make_default(),
            });
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
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(InputPlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(WindowPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
