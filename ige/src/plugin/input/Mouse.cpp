#include "ige/plugin/input/Mouse.hpp"
#include <glm/vec2.hpp>

using glm::vec2;
using ige::plugin::input::Mouse;
using ige::plugin::input::MouseButtonEvent;
using ige::plugin::input::MouseEvent;
using ige::plugin::input::MouseEventType;
using ige::plugin::input::MouseMoveEvent;

void Mouse::clear()
{
    InputRegistry::clear();
    m_scroll = { 0.0f, 0.0f };
    m_movement = { 0.0f, 0.0f };
}

const vec2& Mouse::get_position() const
{
    return m_pos;
}

const vec2& Mouse::get_movement() const
{
    return m_movement;
}

const vec2& Mouse::get_scroll() const
{
    return m_scroll;
}

void Mouse::handle_mouse_event(const MouseEvent& event)
{
    switch (event.type) {
    case MouseEventType::MOUSE_MOVE:
        return handle_event(event.pos);
    case MouseEventType::BUTTON:
        return handle_event(event.button);
    case MouseEventType::SCROLL:
        return handle_event(event.scroll);
    default:
        break;
    }
}

void Mouse::handle_event(const MouseButtonEvent& event)
{
    set_state(event.button, event.state);
}

void Mouse::handle_event(const MouseMoveEvent& event)
{
    vec2 new_pos { event.x, event.y };

    m_movement = new_pos - m_pos;
    m_pos = new_pos;
}

void Mouse::handle_event(const MouseScrollEvent& event)
{
    m_scroll = { event.x, event.y };
}
