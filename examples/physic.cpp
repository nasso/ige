#include "ige.hpp"
#include "ige/plugin/physic/RigidBody.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using glm::vec3;
using glm::vec4;
using ige::asset::Material;
using ige::asset::Mesh;
using ige::asset::Texture;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::EntityId;
using ige::ecs::Schedule;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::physic::PhysicPlugin;
using ige::plugin::physic::RigidBody;
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
    std::optional<EntityId> m_cube;

    void on_start(App& app) override
    {
        auto mesh = Mesh::make_cube(1.0f);
        auto material = Material::make_default();
        material->set(
            "base_color_factor",
            vec4 {
                1.0f,
                0.75f,
                0.35f,
                1.0f,
            });

        auto camera = app.world().create_entity(
            Transform::from_pos(vec3(5.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        app.world()
            .create_entity(
                RigidBody { 0 }, Transform {}, MeshRenderer { mesh, material })
            .id();

        app.world()
            .create_entity(
                RigidBody { 0, true }, Transform::from_pos(vec3(0, 3, 0)),
                MeshRenderer { mesh, material })
            .id();

        m_cube = app.world()
                     .create_entity(
                         Transform::from_pos(vec3(0, 6, 0)), RigidBody {},
                         MeshRenderer { mesh, material })
                     .id();

        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
        auto manager = app.world().get<InputManager>();

        if (manager->keyboard().is_pressed(KeyboardKey::KEY_SPACE)) {
            app.world().get_component<RigidBody>(*m_cube)->apply_force(2.f);
        }
    }
};

int main()
{
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(TransformPlugin {})
        .add_plugin(PhysicPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(InputPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
