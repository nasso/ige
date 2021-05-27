#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <chrono>
#include <iostream>
#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec2;
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
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class PlayerController : public CppBehaviour {
public:
    void tick() override
    {
        auto input = get_resource<InputManager>();

        vec3 direction { 0.0f };

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_UP)) {
            direction.z -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_DOWN)) {
            direction.z += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_RIGHT)) {
            direction.x += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_LEFT)) {
            direction.x -= 1.0f;
        }

        if (direction != vec3 { 0.0f }) {
            auto xform = get_component<Transform>();

            xform->translate(glm::normalize(direction) * 0.05f);
        }
    }
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        auto cube_mesh = Mesh::make_cube(1.0f);
        auto ground_mat = Material::make_default();
        ground_mat->set("base_color_factor", vec4 { 1.0f, 0.5f, 0.85f, 1.0f });

        app.world().create_entity(
            Transform {}
                .set_translation(vec3 { 0.0f, -0.1f, 0.0f })
                .set_scale(vec3 { 10.0f, 0.2f, 10.0f }),
            MeshRenderer {
                cube_mesh,
                ground_mat,
            });

        app.world().create_entity(
            Transform::from_pos({ 0.0f, 0.5f, 0.0f }),
            MeshRenderer {
                cube_mesh,
                Material::make_default(),
            },
            Scripts::from(PlayerController {}));

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
        .add_plugin(InputPlugin {})
        .add_plugin(TimePlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(ScriptPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
