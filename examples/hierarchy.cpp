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
using ige::plugin::Children;
using ige::plugin::MeshRenderer;
using ige::plugin::Parent;
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
    std::vector<World::EntityRef> children;

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
            Transform::from_pos({ 3.0f, 2.0f, 0.0f }).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        // the mesh and material for each cube
        auto cube_mesh = Mesh::make_cube(1.0f);
        auto cube_mat = Material::load_default();

        root_cube = app.world().create_entity(
            Transform {}, MeshRenderer { cube_mesh, cube_mat });

        auto satellite = app.world().create_entity(
            Transform::from_pos({ 2.0f, 0.0f, 0.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { root_cube->id() });

        children.push_back(app.world().create_entity(
            Transform::from_pos({ 1.0f, 0.0f, 0.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite.id() }));
        children.push_back(app.world().create_entity(
            Transform::from_pos({ 0.0f, 1.0f, 0.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite.id() }));
        children.push_back(app.world().create_entity(
            Transform::from_pos({ 0.0f, 0.0f, 1.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite.id() }));
    }

    void on_update(App& app) override
    {
        Instant now = std::chrono::steady_clock::now();
        std::chrono::duration<float> dur = now - start_time;
        float t = dur.count();

        // make the root cube move & rotate
        if (root_cube) {
            root_cube->get_component<Transform>()->get().set_rotation(
                vec3(0.0f, t, 0.0f));

            if (t > 5.0f) {
                root_cube->remove();
                root_cube.reset();
            }
        }

        // make the little cubes rotate too!!
        for (auto cube : children) {
            cube.get_component<Transform>()->get().set_rotation(
                vec3(0.0f, t, t));
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
