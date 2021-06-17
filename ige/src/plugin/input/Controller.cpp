#include "ige/plugin/input/Controller.hpp"

using ige::plugin::input::Controller;
using ige::plugin::input::ControllerEvent;
using ige::plugin::input::ControllerEventType;
using ige::plugin::input::ControllerId;

Controller::Controller(const ControllerId& id)
{
    m_id = id;
}

ControllerId Controller::id() const
{
    return m_id;
}

void Controller::set_axis_value(ControllerAxis axis, float value)
{
    m_axes[axis] = value;
}

float Controller::get_axis_value(ControllerAxis axis)
{
    return m_axes[axis];
}
