#include "ige/plugin/InputPlugin.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/input/InputManager.hpp"

using ige::core::App;
using ige::core::EventChannel;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::InputPlugin;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputManager;

void InputPlugin::plug(App::Builder& builder) const
{
    builder.emplace<InputManager>();
    builder.emplace<EventChannel<InputEvent>>();
    builder.add_system(System(InputManager::input_manager_updater));
}
