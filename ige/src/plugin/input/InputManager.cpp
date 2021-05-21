#include "ige/plugin/input/InputManager.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/World.hpp"

using ige::core::EventChannel;
using ige::ecs::World;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::Keyboard;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::Mouse;

InputManager::InputManager()
{
}

Keyboard& InputManager::keyboard()
{
    return m_keyboard;
}

Mouse& InputManager::mouse()
{
    return m_mouse;
}

void InputManager::input_manager_updater(ecs::World& wld)
{
    auto opt_manager = wld.get<InputManager>();

    if (!opt_manager)
        return;
    auto& manager = opt_manager->get();

    if (!manager.m_events) {
        auto opt_events_channel = wld.get<EventChannel<InputEvent>>();

        if (!opt_events_channel)
            return;
        manager.m_events.emplace(opt_events_channel->get().subscribe());
    }
    manager.handle_new_events();
}

void InputManager::handle_new_events()
{
    while (const auto& opt_event = m_events->next_event()) {
        auto& event = opt_event->get();
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
}
