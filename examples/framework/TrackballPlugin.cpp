#include "TrackballPlugin.hpp"
#include <glm/vec3.hpp>
#include <ige.hpp>
#include <iostream>

using ige::core::App;
using ige::ecs::EntityId;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::input::InputManager;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;
using ige::plugin::time::Time;
using ige::plugin::transform::Transform;

void rotate_camera(TrackballController& tb, float theta, float phi)
{
    tb.theta += glm::sign(tb.up) * theta;
    tb.phi += phi;

    // keep phi in [-2PI, +2PI]
    const float tp = glm::two_pi<float>();
    if (tb.phi > tp) {
        tb.phi -= tp;
    } else if (tb.phi < -tp) {
        tb.phi += tp;
    }

    const float pi = glm::pi<float>();
    tb.up = (0 < tb.phi && tb.phi < pi) || (-tp < tb.phi && tb.phi < -pi)
        ? 1.0f
        : -1.0f;
}

void pan_camera(
    TrackballController& tb, const Transform& xform, float dx, float dy)
{
    // compute the 3 vectors defining the space of our camera
    glm::vec3 forward = glm::normalize(tb.target - xform.translation());
    glm::vec3 right = glm::cross(forward, glm::vec3 { 0.0f, tb.up, 0.0f });
    glm::vec3 up = glm::cross(forward, right);

    tb.target += right * dx;
    tb.target += up * dy;
}

void update_transform(const TrackballController& tb, Transform& xform)
{
    xform.set_translation(tb.target);
    xform.translate(
        glm::vec3 {
            glm::sin(tb.phi) * glm::sin(tb.theta),
            glm::cos(tb.phi),
            glm::sin(tb.phi) * glm::cos(tb.theta),
        }
        * tb.distance);
    xform.look_at(tb.target, glm::vec3 { 0.0f, tb.up, 0.0f });
}

TrackballController::TrackballController(float distance, float theta, float phi)
    : distance(distance * 2.0f)
    , distance_target(distance)
    , theta(theta)
    , phi(phi)
{
    rotate_camera(*this, 0.0f, 0.0f);
}

void trackball_system(World& world)
{
    auto input = world.get<InputManager<>>();
    auto time = world.get<Time>();

    if (!input) {
        std::cerr << "InputManager resource not found!" << std::endl;
        return;
    }

    const std::uint32_t ticks = time ? time->ticks() : 1;
    const glm::vec2 mouse_mov = -input->mouse().get_movement() * 0.01f;
    const bool mouse_left = input->mouse().is_down(MouseButton::LEFT);
    const bool mouse_middle = input->mouse().is_down(MouseButton::MIDDLE);
    const bool mouse_right = input->mouse().is_down(MouseButton::RIGHT);
    const bool keyboard_tab = input->keyboard().is_down(KeyboardKey::KEY_TAB);
    const float scroll_y = input->mouse().get_scroll().y;

    const auto get_transform_for
        = [&](EntityId entity, bool& needs_update) -> Transform& {
        auto xform_ptr = world.get_component<Transform>(entity);

        if (!xform_ptr) {
            xform_ptr = &world.emplace_component<Transform>(entity);
            needs_update = true;
        }

        return *xform_ptr;
    };

    auto trackballs = world.query<TrackballController>();

    for (auto [entity, tb] : trackballs) {
        bool needs_update = false;

        auto& xform = get_transform_for(entity, needs_update);

        if (mouse_left || mouse_middle) {
            rotate_camera(tb, mouse_mov.x, mouse_mov.y);
            needs_update = true;
        }

        if (mouse_right) {
            pan_camera(tb, xform, mouse_mov.x, mouse_mov.y);
            needs_update = true;
        }

        if (keyboard_tab) {
            tb.target = glm::vec3(0.0f);
            needs_update = true;
        }

        if (scroll_y != 0.0f) {
            tb.distance_target *= 1.0f - scroll_y * 0.1f;
            needs_update = true;
        }

        for (std::uint32_t t = 0; t < ticks; ++t) {
            if (std::abs(tb.distance - tb.distance_target) <= 0.01f) {
                tb.distance = tb.distance_target;
            }

            if (tb.distance != tb.distance_target) {
                tb.distance = tb.distance * 0.9f + tb.distance_target * 0.1f;
                needs_update = true;
            }
        }

        if (needs_update) {
            update_transform(tb, xform);
        }
    }
}

void TrackballPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(trackball_system));
}
