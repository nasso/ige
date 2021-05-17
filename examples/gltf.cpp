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
using ige::asset::Gltf;
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
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<World::EntityRef> m_model;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->get().subscribe());

        try {
            auto model = Gltf::from_bin("gltf/BoxTextured.glb");
            m_model = model.create_entities(app.world());
        } catch (const std::exception& e) {
            std::cerr << "Couldn't load model: " << e.what() << std::endl;
        }

        auto camera = app.world().create_entity();
        camera.add_component(
            Transform::make_look_at(vec3(2.0f, 1.0f, 0.0f), vec3(0.0f)));
        camera.emplace_component<PerspectiveCamera>(90.0f);
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
            .add_plugin(TransformPlugin {})
            .add_plugin(RenderingPlugin {})
            .add_plugin(WindowingPlugin {})
            .run<RootState>();
        std::cout << "Bye bye!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "[FATAL ERROR] " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[FATAL ERROR] Unknown object caught in main()"
                  << std::endl;
    }
}
