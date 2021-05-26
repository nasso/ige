#include "ige.hpp"
#include <chrono>
#include <iostream>
#include <optional>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec2;
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
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;
using ige::plugin::render::MeshRenderer;
using ige::plugin::render::PerspectiveCamera;
using ige::plugin::render::RenderPlugin;
using ige::plugin::script::CppBehaviour;
using ige::plugin::script::ScriptPlugin;
using ige::plugin::script::Scripts;
using ige::plugin::transform::Transform;
using ige::plugin::transform::TransformPlugin;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class PlayerController : public CppBehaviour {
public:
    void update() override
    {
        auto input = get_resource<InputManager>();

        vec3 direction { 0.0f };

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_UP)) {
            direction.z += 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_DOWN)) {
            direction.z -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_RIGHT)) {
            direction.x -= 1.0f;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_ARROW_LEFT)) {
            direction.x += 1.0f;
        }

        if (direction != vec3 { 0.0f }) {
            auto xform = get_component<Transform>();

            xform->translate(glm::normalize(direction) * 0.01f);
        }
    }
};

class TrackballCamera : public CppBehaviour {
private:
    vec3 m_target { 0.0f };
    float m_distance = 10.0f;
    float m_theta = 0.0f;
    float m_phi = 45.0f;
    float m_up = 1.0f;

    void rotate_camera(float theta, float phi)
    {
        m_theta += glm::sign(m_up) * theta;
        m_phi += phi;

        // keep phi in [-2PI, +2PI]
        const float tp = glm::two_pi<float>();
        if (m_phi > tp) {
            m_phi -= tp;
        } else if (m_phi < -tp) {
            m_phi += tp;
        }

        const float pi = glm::pi<float>();
        m_up = (0 < m_phi && m_phi < pi) || (-tp < m_phi && m_phi < -pi)
            ? 1.0f
            : -1.0f;
    }

    void pan_camera(float dx, float dy)
    {
        auto xform = get_component<Transform>();

        // compute the 3 vectors defining the space of our camera
        vec3 forward = glm::normalize(m_target - xform->translation());
        vec3 right = glm::cross(forward, vec3 { 0.0f, m_up, 0.0f });
        vec3 up = glm::cross(forward, right);

        m_target += right * dx;
        m_target += up * dy;
    }

    void update_transform()
    {
        auto xform = get_component<Transform>();

        xform->set_translation(m_target);
        xform->translate(
            vec3 {
                glm::sin(m_phi) * glm::sin(m_theta),
                glm::cos(m_phi),
                glm::sin(m_phi) * glm::cos(m_theta),
            }
            * m_distance);
        xform->look_at(m_target, vec3 { 0.0f, m_up, 0.0f });
    }

public:
    TrackballCamera(float distance = 10.0f)
        : m_distance(distance)
    {
    }

    void on_start() override
    {
        auto xform = emplace_component<Transform>();

        update_transform();
    }

    void update() override
    {
        auto input = get_resource<InputManager>();

        bool needs_update = false;

        if (input->mouse().is_down(MouseButton::LEFT)
            || input->mouse().is_down(MouseButton::MIDDLE)) {
            auto mov = -input->mouse().get_movement() * 0.01f;

            rotate_camera(mov.x, mov.y);
            needs_update = true;
        }

        if (input->mouse().is_down(MouseButton::RIGHT)) {
            auto mov = -input->mouse().get_movement() * 0.01f;

            pan_camera(mov.x, mov.y);
            needs_update = true;
        }

        if (input->keyboard().is_down(KeyboardKey::KEY_TAB)) {
            m_target = vec3 { 0.0f };
            needs_update = true;
        }

        const float scroll_y = input->mouse().get_scroll().y;
        if (scroll_y != 0.0f) {
            m_distance *= 1.0f - scroll_y * 0.1f;
            needs_update = true;
        }

        if (needs_update) {
            update_transform();
        }
    }
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(channel->subscribe());

        auto camera = app.world().create_entity(
            PerspectiveCamera(90.0f), Scripts::from(TrackballCamera { 3.0f }));

        app.world().create_entity(
            Transform {},
            MeshRenderer {
                Mesh::make_cube(1.0f),
                Material::make_default(),
            },
            Scripts::from(PlayerController {}));
    }

    void on_update(App& app) override
    {
        while (const auto& event = m_win_events->next_event()) {
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
        .add_plugin(InputPlugin {})
        .add_plugin(TransformPlugin {})
        .add_plugin(WindowPlugin {})
        .add_plugin(RenderPlugin {})
        .add_plugin(ScriptPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
