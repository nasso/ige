#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <chrono>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>

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
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RemoveInOneSec : public CppBehaviour {
public:
    float timer = 0.0f;

    RemoveInOneSec()
    {
        std::cout << "RemoveInOneSec()" << std::endl;
    }

    void on_start() override
    {
        std::cout << "start" << std::endl;
    }

    void tick() override
    {
        timer += get_resource<Time>()->tick_seconds();

        if (timer >= 1.0f) {
            world().remove_entity(entity());
        }
    }
};

class CharacterController : public CppBehaviour {
public:
    vec2 direction { 0.0f };
    bool running = false;

    void tick() override
    {
        auto xform = get_component<Transform>();

        if (direction != vec2 { 0.0f }) {
            vec2 velocity = glm::normalize(direction) * 0.05f;

            if (running) {
                velocity *= 2.0f;
            }

            xform->translate(vec3 { velocity.x, 0.0f, velocity.y });
        }

        xform->rotate(vec3 { 0.0f, 1.0f, 0.0f });
    }
};

class PlayerController : public CppBehaviour {
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_mat;

public:
    PlayerController(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> mat)
        : m_mesh(mesh)
        , m_mat(mat)
    {
    }

    void update() override
    {
        auto input = get_resource<InputManager>();
        auto controller = get_script<CharacterController>();

        if (!controller) {
            std::cout << "no character controller found!" << std::endl;
            return;
        }

        controller->running
            = input->keyboard().is_down(KeyboardKey::KEY_SHIFT_LEFT);

        controller->direction = { 0.0f, 0.0f };

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_UP)) {
            controller->direction.y -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_DOWN)) {
            controller->direction.y += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_RIGHT)) {
            controller->direction.x += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_LEFT)) {
            controller->direction.x -= 1.0f;
        }

        if (input->keyboard().is_pressed(KeyboardKey::KEY_SPACE)) {
            auto xform = get_component<Transform>();
            auto pos = xform->world_translation();

            world().create_entity(
                Transform::from_pos({ pos.x, pos.y + 1.0f, pos.z }),
                MeshRenderer { m_mesh, m_mat },
                Scripts::from(RemoveInOneSec {}));
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
        auto cube_mat = Material::make_default();
        auto ground_mat = Material::make_default();
        ground_mat->set("base_color_factor", vec4 { 1.0f, 0.5f, 0.85f, 1.0f });

        app.world().create_entity(
            Transform {}
                .set_translation(vec3 { 0.0f, -0.1f, 0.0f })
                .set_scale(vec3 { 10.0f, 0.2f, 10.0f }),
            MeshRenderer { cube_mesh, ground_mat });

        app.world().create_entity(
            Transform::from_pos({ 0.0f, 0.5f, 0.0f }),
            MeshRenderer { cube_mesh, cube_mat },
            Scripts::from(
                PlayerController { cube_mesh, cube_mat },
                CharacterController {}));

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
