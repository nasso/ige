#include "ige/plugin/input/Mouse.hpp"

using ige::plugin::input::Mouse;
using ige::plugin::input::MouseEvent;
using ige::plugin::input::MouseEventButton;
using ige::plugin::input::MouseEventPos;
using ige::plugin::input::MouseEventType;

void Mouse::set_pos(double xpos, double ypos)
{
    m_xpos = xpos;
    m_ypos = ypos;
}

Mouse::MousePos Mouse::get_pos() const
{
    return { m_xpos, m_ypos };
}

void Mouse::handle_mouse_event(MouseEvent event)
{
    switch (event.type) {
    case MouseEventType::MOUSEPOS:
        handle_position_event(event.pos);
        break;
    case MouseEventType::BUTTON:
        handle_button_event(event.button);
        break;

    default:
        break;
    }
}

void Mouse::handle_button_event(MouseEventButton event)
{
    set_state(event.button, event.state);
}

void Mouse::handle_position_event(MouseEventPos event)
{
    m_xpos = event.xpos;
    m_ypos = event.ypos;
}
