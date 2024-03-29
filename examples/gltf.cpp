#include "TrackballCamera.hpp"
#include "ige.hpp"
#include <exception>
#include <fstream>
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
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::render::Light;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
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

struct Rotation {
    vec3 velocity;
};

static void rotation_system(World& world)
{
    auto time = world.get<Time>();

    for (auto [entity, rot, xform] : world.query<Rotation, Transform>()) {
        xform.rotate(rot.velocity * time->delta_seconds());
    }
}

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<EntityId> entity_to_edit;
    std::optional<EntityId> entity_to_remove;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        // create model
        entity_to_edit = app.world().create_entity(
            Transform::from_pos(vec3(-2.0f, 0.0f, 0.0f)),
            Rotation {
                vec3(20.0f, 0.0f, 0.0f),
            },
            GltfScene {
                "assets/BoxTextured.glb",
                GltfFormat::BINARY,
            });

        entity_to_remove = app.world().create_entity(
            Transform::from_pos(vec3(0.0f, 0.0f, 0.0f)),
            Rotation {
                vec3(0.0f, 20.0f, 0.0f),
            },
            GltfScene {
                "assets/BoxInterleaved.glb",
                GltfFormat::BINARY,
            });

        app.world().create_entity(
            Transform::from_pos(vec3(2.0f, 0.0f, 0.0f)).set_scale(0.1f),
            Rotation {
                vec3(0.0f, 0.0f, 20.0f),
            },
            GltfScene {
                "assets/OrientationTest.glb",
                GltfFormat::BINARY,
            });

        // create camera
        app.world().create_entity(
            PerspectiveCamera(90.0f),
            Scripts::from(TrackballCamera { 5.0f, 0.0f }));

        // ambient light
        app.world().create_entity(Light::ambient(0.2f));

        // sun
        app.world().create_entity(
            Transform {}.set_rotation(vec3 { 45.0f, 45.0f, 0.0f }),
            Light::directional(0.8f));
    }

    void on_update(App& app) override
    {
        auto input = app.world().get<InputManager<>>();

        if (input->keyboard().is_pressed(KeyboardKey::KEY_DELETE)) {
            app.world().remove_component<GltfScene>(*entity_to_edit);
            app.world().remove_entity(*entity_to_remove);
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
    try {
        std::cout << "Starting application..." << std::endl;
        App::Builder()
            .insert(WindowSettings { "Hello, World!", 800, 600 })
            .add_plugin(TimePlugin {})
            .add_plugin(GltfPlugin {})
            .add_plugin(TransformPlugin {})
            .add_plugin(WindowPlugin {})
            .add_plugin(RenderPlugin {})
            .add_plugin(InputPlugin<> {})
            .add_plugin(ScriptPlugin {})
            .add_system(System::from(rotation_system))
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
