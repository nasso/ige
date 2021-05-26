#include "TrackballCamera.hpp"
#include "ige/plugin/InputPlugin.hpp"
#include "ige/plugin/ScriptPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"

#include <glm/vec3.hpp>
; // TODO: https://bit.ly/3hhMJ58

using glm::vec3;
using ige::plugin::input::InputManager;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;
using ige::plugin::transform::Transform;

void TrackballCamera::rotate_camera(float theta, float phi)
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
    m_up = (0 < m_phi && m_phi < pi) || (-tp < m_phi && m_phi < -pi) ? 1.0f
                                                                     : -1.0f;
}

void TrackballCamera::pan_camera(float dx, float dy)
{
    auto xform = get_component<Transform>();

    // compute the 3 vectors defining the space of our camera
    vec3 forward = glm::normalize(m_target - xform->translation());
    vec3 right = glm::cross(forward, vec3 { 0.0f, m_up, 0.0f });
    vec3 up = glm::cross(forward, right);

    m_target += right * dx;
    m_target += up * dy;
}

void TrackballCamera::update_transform()
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

TrackballCamera::TrackballCamera(float distance)
    : m_distance(distance)
{
}

void TrackballCamera::on_start()
{
    auto xform = emplace_component<Transform>();

    update_transform();
}

void TrackballCamera::update()
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
