#include "ige.hpp"
#include <optional>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec3;
using glm::vec4;
using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::game::Material;
using ige::game::Mesh;
using ige::game::MeshRenderer;
using ige::game::PerspectiveCamera;
using ige::game::RenderingPlugin;
using ige::game::Texture;
using ige::game::Transform;
using ige::game::WindowEvent;
using ige::game::WindowEventKind;
using ige::game::WindowingPlugin;
using ige::game::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto mesh = Mesh::make_cube(1.0f);
        auto material = Material::load_default();
        auto texture = Texture::load_file("examples/texture.png");

        material->set("base_color", texture);

        auto camera = app.world().create_entity();
        camera.add_component(Transform::look_at(vec3(3.0f), vec3(0.0f)));
        camera.add_component(PerspectiveCamera(90.0f));

        auto cube = app.world().create_entity();
        cube.add_component(Transform {});
        cube.add_component(MeshRenderer { mesh, material });

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
    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(RenderingPlugin {})
        .add_plugin(WindowingPlugin {})
        .run<RootState>();
}
