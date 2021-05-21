#include "ige/plugin/input/Keyboard.hpp"

using ige::plugin::input::Keyboard;

void Keyboard::handle_keyboard_event(const KeyboardEvent& event)
{
    set_state(event.key, event.state);
}
