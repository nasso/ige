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
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::CppBehaviour;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class PlayerController : public CppBehaviour {
public:
    void update() override
    {
        auto input = get_resource<InputManager>();
        auto xform = get_component<Transform>();

        vec3 direction { 0.0f };

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_UP)) {
            direction.z += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_DOWN)) {
            direction.z -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_RIGHT)) {
            direction.x -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_LEFT)) {
            direction.x += 1.0f;
        }

        if (direction != vec3 { 0.0f }) {
            xform->translate(glm::normalize(direction) * 0.01f);
        }
    }
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        auto camera = app.world().create_entity(
            Transform::from_pos(vec3(0.0f, 3.0f, -3.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        app.world().create_entity(
            Transform {},
            MeshRenderer {
                Mesh::make_cube(1.0f),
                Material::make_default(),
            },
            Scripts {
                PlayerController {},
            });
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
        .add_plugin(InputPlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(ScriptPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
