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

struct InputSubscription {
    EventChannel<InputEvent>::Subscription sub;

    InputSubscription(EventChannel<InputEvent>& chan)
        : sub(chan.subscribe())
    {
    }
};

static void update_input_manager(World& wld)
{
    auto& manager = wld.get_or_emplace<InputManager>();
    auto& channel = wld.get_or_emplace<EventChannel<InputEvent>>();
    auto& sub = wld.get_or_emplace<InputSubscription>(channel).sub;

    manager.reset();
    while (const auto& event = sub.next_event()) {
        manager.push_event(*event);
    }
}

void InputPlugin::plug(App::Builder& builder) const
{
    builder.emplace<InputManager>();
    builder.emplace<EventChannel<InputEvent>>();
    builder.add_system(System(update_input_manager));
}
