#include "ige/plugin/input/InputManager.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/World.hpp"

using ige::core::EventChannel;
using ige::ecs::World;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::Keyboard;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::Mouse;

Keyboard& InputManager::keyboard()
{
    return m_keyboard;
}

const Keyboard& InputManager::keyboard() const
{
    return m_keyboard;
}

Mouse& InputManager::mouse()
{
    return m_mouse;
}

const Mouse& InputManager::mouse() const
{
    return m_mouse;
}

void InputManager::reset()
{
    m_keyboard.clear();
    m_mouse.clear();
}

void InputManager::push_event(const InputEvent& event)
{
    switch (event.type) {
    case InputEventType::KEYBOARD:
        m_keyboard.handle_keyboard_event(event.keyboard);
        break;
    case InputEventType::MOUSE:
        m_mouse.handle_mouse_event(event.mouse);
        break;
    default:
        break;
    }
}
