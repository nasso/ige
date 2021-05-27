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
using ige::ecs::Schedule;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::animation::AnimationPlugin;
using ige::plugin::animation::Animator;
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;
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

class ExampleScript : public CppBehaviour {
public:
    void update() override
    {
        auto input = get_resource<InputManager>();
        auto scene = get_component<GltfScene>();
        auto animator = get_or_emplace_component<Animator>();

        if (!animator->current_animation && scene->has_loaded()) {
            auto animation = scene->get_animation(0);

            animator->current_animation = animation;
        }

        if (input->keyboard().is_pressed(KeyboardKey::KEY_ARROW_LEFT)) {
            animator->current_time = Animator::Duration::zero();
        }

        if (input->keyboard().is_pressed(KeyboardKey::KEY_SPACE)) {
            animator->paused = !animator->paused;
        }
    }
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        // create model
        app.world().create_entity(
            Transform {}.set_scale(0.05f),
            GltfScene { "assets/Fox.glb", GltfFormat::BINARY },
            Scripts::from(ExampleScript {}));

        // create camera
        app.world().create_entity(
            PerspectiveCamera(90.0f), Scripts::from(TrackballCamera { 10.0f }));
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
            .add_plugin(TimePlugin {})
            .add_plugin(GltfPlugin {})
            .add_plugin(TransformPlugin {})
            .add_plugin(WindowPlugin {})
            .add_plugin(RenderPlugin {})
            .add_plugin(InputPlugin {})
            .add_plugin(ScriptPlugin {})
            .add_plugin(AnimationPlugin {})
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
