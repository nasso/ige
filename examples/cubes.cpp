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

    void on_start(App& app) override
    {
        auto mesh = Mesh::make_cube(1.0f);
        auto material = Material::load_default();

        auto camera = app.world().create_entity();
        camera.add_component(
            Transform::make_look_at(vec3(2.0f, 2.0f, 0.0f), vec3(0.0f)));
        camera.emplace_component<PerspectiveCamera>(90.0f);

        for (int x = -1; x <= 1; x++) {
            for (int y = -1; y <= 1; y++) {
                auto cube = app.world().create_entity();
                auto& xform = cube.emplace_component<Transform>();
                cube.emplace_component<MeshRenderer>(mesh, material);

                xform.set_translation(vec3(x, 0.0f, y));
            }
        }

        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->get().subscribe());
    }

    void on_update(App& app) override
    {
        while (const auto& event = m_win_events->next_event()) {
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
