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
using ige::plugin::gltf::GltfFormat;
using ige::plugin::gltf::GltfPlugin;
using ige::plugin::gltf::GltfScene;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<World::EntityRef> m_model;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->get().subscribe());

        // create model
        m_model = app.world().create_entity(
            Transform {}.set_scale(0.5f),
            GltfScene { "assets/OrientationTest.glb", GltfFormat::BINARY });

        // create camera
        app.world().create_entity(
            Transform::from_pos(vec3(8.0f, 5.0f, 8.0f)).look_at(vec3(0.0f)),
            PerspectiveCamera(90.0f));
    }

    void on_update(App& app) override
    {
        if (m_model) {
            auto& xform = m_model->get_or_emplace_component<Transform>();

            xform.rotate(vec3 { 0.0f, 0.2f, 0.0f });
        }

        while (const auto& event = m_win_events->next_event()) {
            if (event->get().kind == WindowEventKind::WindowClose) {
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
            .add_plugin(WindowingPlugin {})
            .add_plugin(RenderingPlugin {})
            .add_plugin(TransformPlugin {})
            .add_plugin(GltfPlugin {})
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
