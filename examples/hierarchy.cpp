#include "ige.hpp"
#include <chrono>
#include <iostream>
#include <optional>

#include <glm/trigonometric.hpp>
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
using ige::plugin::MeshRenderer;
using ige::plugin::PerspectiveCamera;
using ige::plugin::RenderingPlugin;
using ige::plugin::Transform;
using ige::plugin::TransformPlugin;
using ige::plugin::WindowEvent;
using ige::plugin::WindowEventKind;
using ige::plugin::WindowingPlugin;
using ige::plugin::WindowSettings;

class RootState : public State {
    using Instant = std::chrono::time_point<std::chrono::steady_clock>;

    std::optional<EventChannel<WindowEvent>::Subscription> win_events;

    std::optional<World::EntityRef> root_cube;

    Instant start_time;

    void on_start(App& app) override
    {
        start_time = std::chrono::steady_clock::now();

        // subscribe to the window event channel
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        if (channel) {
            win_events.emplace(channel->get().subscribe());
        }

        // create camera
        app.world().create_entity(
            Transform::make_look_at(vec3(-3.0f, 3.0f, 0.0f), vec3(0.0f)),
            PerspectiveCamera(90.0f));

        // the mesh and material for each cube
        auto cube_mesh = Mesh::make_cube(1.0f);
        auto cube_mat = Material::load_default();

        root_cube = app.world().create_entity(
            Transform {}, MeshRenderer { cube_mesh, cube_mat });
    }

    void on_update(App& app) override
    {
        Instant now = std::chrono::steady_clock::now();
        std::chrono::duration<float> dur = now - start_time;
        float t = dur.count();

        // make the root cube move & rotate
        if (root_cube) {
            auto [xform] = *root_cube->get_component_bundle<Transform>();

            xform.set_rotation(vec3(t, 0.0f, t));
            xform.set_translation({
                glm::cos(t) * 2.0f,
                0.0f,
                glm::sin(t) * 2.0f,
            });
        }

        // quit the app when the window closes
        while (const auto& event = win_events->next_event()) {
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
        .add_plugin(TransformPlugin {})
        .add_plugin(RenderingPlugin {})
        .add_plugin(WindowingPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
