#include "ige/plugin/input/InputManager.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/World.hpp"

using ige::core::EventChannel;
using ige::ecs::World;
using ige::plugin::input::Controller;
using ige::plugin::input::ControllerEvent;
using ige::plugin::input::ControllerEventType;
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

Controller& InputManager::add_controller(const ControllerId& id)
{
    return m_controllers.emplace(id, id).first->second;
}

std::vector<Controller*> InputManager::controllers()
{
    std::vector<Controller*> res;

    for (auto& [controller_id, controller] : m_controllers) {
        res.push_back(&controller);
    }
    return res;
}

std::vector<const Controller*> InputManager::controllers() const
{
    std::vector<const Controller*> res;

    for (auto& [controller_id, controller] : m_controllers) {
        res.push_back(&controller);
    }
    return res;
}

Controller* InputManager::controller(const ControllerId& id)
{
    auto controller = m_controllers.find(id);

    if (controller == m_controllers.end()) {
        return nullptr;
    }
    return &controller->second;
}

const Controller* InputManager::controller(const ControllerId& id) const
{
    auto controller = m_controllers.find(id);

    if (controller == m_controllers.end()) {
        return nullptr;
    }
    return &controller->second;
}

void InputManager::reset()
{
    m_keyboard.clear();
    m_mouse.clear();
    for (auto& [controller_id, controller] : m_controllers) {
        controller.clear();
    }
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
