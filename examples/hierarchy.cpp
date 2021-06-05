#include "ige.hpp"
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
using ige::ecs::EntityId;
using ige::ecs::Schedule;
using ige::ecs::World;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::time::Time;
using ige::plugin::time::TimePlugin;
using ige::plugin::transform::Children;
using ige::plugin::transform::Parent;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> win_events;
    std::optional<EntityId> root_cube;
    std::vector<EntityId> children;

    void on_start(App& app) override
    {
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
            MeshRenderer { cube_mesh, cube_mat }, Parent { *root_cube });

        children.push_back(app.world().create_entity(
            Transform::from_pos({ 1.0f, 0.0f, 0.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite }));
        children.push_back(app.world().create_entity(
            Transform::from_pos({ 0.0f, 1.0f, 0.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite }));
        children.push_back(app.world().create_entity(
            Transform::from_pos({ 0.0f, 0.0f, 1.0f }).set_scale(0.5f),
            MeshRenderer { cube_mesh, cube_mat }, Parent { satellite }));
    }

    void on_update(App& app) override
    {
        auto time = app.world().get<Time>();

        if (!time) {
            std::cerr << "Time isn't real :(" << std::endl;
            return;
        }

        // make the root cube move & rotate
        if (root_cube) {
            app.world()
                .get_component<Transform>(*root_cube)
                ->set_rotation(vec3(0.0f, 60.0f * time->now_seconds(), 0.0f));

            if (time->now_seconds() > 5.0f) {
                app.world().remove_entity(*root_cube);
                root_cube.reset();
            }
        }

        // make the little cubes rotate too!!
        for (auto cube : children) {
            app.world().get_component<Transform>(cube)->set_rotation(vec3 {
                0.0f,
                60.0f * time->now_seconds(),
                60.0f * time->now_seconds(),
            });
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
        .add_plugin(TimePlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(RenderPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
