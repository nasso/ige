#include "ige/plugin/input/Mouse.hpp"

using ige::plugin::input::Mouse;
using ige::plugin::input::MouseButtonEvent;
using ige::plugin::input::MouseEvent;
using ige::plugin::input::MouseEventType;
using ige::plugin::input::MouseMoveEvent;

void Mouse::set_position(float xpos, float ypos)
{
    m_xpos = xpos;
    m_ypos = ypos;
}

Mouse::MousePos Mouse::get_position() const
{
    return { m_xpos, m_ypos };
}

void Mouse::handle_mouse_event(const MouseEvent& event)
{
    switch (event.type) {
    case MouseEventType::MOUSE_MOVE:
        handle_position_event(event.pos);
        break;
    case MouseEventType::BUTTON:
        handle_button_event(event.button);
        break;

    default:
        break;
    }
}

void Mouse::handle_button_event(const MouseButtonEvent& event)
{
    set_state(event.button, event.state);
}

void Mouse::handle_position_event(const MouseMoveEvent& event)
{
    m_xpos = event.x;
    m_ypos = event.y;
}
