#include "ige.hpp"
#include <exception>
#include <fstream>
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
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

struct Rotation {
    vec3 velocity;
};

static void rotation_system(World& world)
{
    for (auto [entity, rot, xform] : world.query<Rotation, Transform>()) {
        xform.rotate(rot.velocity);
    }
}

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        // create model
        app.world().create_entity(
            Transform::from_pos(vec3(-2.0f, 0.0f, 0.0f)),
            Rotation {
                vec3(0.0f, 0.2f, 0.0f),
            },
            GltfScene {
                "assets/BoxTextured.glb",
                GltfFormat::BINARY,
            });

        app.world().create_entity(
            Transform::from_pos(vec3(2.0f, 0.0f, 0.0f)).set_scale(0.2f),
            Rotation {
                vec3(0.0f, 0.2f, 0.0f),
            },
            GltfScene {
                "assets/OrientationTest.glb",
                GltfFormat::BINARY,
            });

        // create camera
        app.world().create_entity(
            Transform::from_pos(vec3(0.0f, 2.0f, 4.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }
    }
};

int main()
{
    try {
        std::cout << "Starting application..." << std::endl;
        App::Builder()
            .insert(WindowSettings { "Hello, World!", 800, 600 })
            .add_system(System(rotation_system))
            .add_plugin(GltfPlugin {})
            .add_plugin(TransformPlugin {})
            .add_plugin(WindowPlugin {})
            .add_plugin(RenderPlugin {})
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
