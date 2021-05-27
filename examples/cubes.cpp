#include "ige.hpp"
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
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    std::vector<World::EntityRef> cubes;

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
            Transform::from_pos(vec3(-3.0f, 3.0f, 0.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                auto cube = app.world().create_entity();
                auto& xform = cube.emplace_component<Transform>();
                cube.emplace_component<MeshRenderer>(mesh, material);
                cubes.push_back(cube);

                xform.set_translation({ x * 1.5f, 0.0f, y * 1.5f });
            }
        }

        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());
    }

    void on_update(App& app) override
    {
        float t = app.world().get<Time>()->seconds_since_startup();

        for (auto cube : cubes) {
            Transform* xform = cube.get_component<Transform>();

            xform->set_rotation(vec3(t, 0.0f, t));

            t /= 2.0f;
        }

        while (auto event = m_win_events->next_event()) {
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
        .add_plugin(TransformPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(TimePlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
