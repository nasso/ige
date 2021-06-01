#include "ige.hpp"
#include <chrono>
#include <glm/trigonometric.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <optional>

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
using ige::plugin::transform::Children;
using ige::plugin::transform::Parent;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

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
        if (auto channel = app.world().get<EventChannel<WindowEvent>>()) {
            win_events.emplace(channel->subscribe());
        }

        // create camera
        app.world().create_entity(
            Transform::from_pos({ 3.0f, 2.0f, 0.0f }).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));

        // the mesh and material for each cube
        auto cube_mesh = Mesh::make_cube(1.0f);
        auto cube_mat = Material::make_default();

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
            root_cube->get_component<Transform>()->set_rotation(
                vec3(0.0f, t, 0.0f));

            if (t > 5.0f) {
                root_cube->remove();
                root_cube.reset();
            }
        }

        // make the little cubes rotate too!!
        for (auto cube : children) {
            cube.get_component<Transform>()->set_rotation(vec3(0.0f, t, t));
        }

        // quit the app when the window closes
        while (auto event = win_events->next_event()) {
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
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
