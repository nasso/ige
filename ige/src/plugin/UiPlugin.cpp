#include "igepch.hpp"

#include "ige/core/App.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/InputPlugin.hpp"
#include "ige/plugin/TransformPlugin.hpp"
#include "ige/plugin/UiPlugin.hpp"
#include "ige/plugin/WindowPlugin.hpp"

using glm::vec2;
using ige::core::App;
using ige::core::Event;
using ige::core::EventChannel;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::MouseEventType;
using ige::plugin::transform::RectTransform;
using ige::plugin::ui::EventTarget;
using ige::plugin::ui::UiPlugin;
using ige::plugin::window::WindowInfo;

struct MouseMovement {
    bool entered = false;
    bool down = false;
    vec2 pos;

    MouseMovement(vec2 entry_pos)
        : pos(entry_pos)
    {
    }
};

template <Event E>
static EventChannel<E>::Subscription make_subscription(World& world)
{
    auto channel = world.get<EventChannel<E>>();

    if (!channel) {
        throw std::runtime_error("No input event channel found!");
    }

    return channel->subscribe();
}

struct UiEvents {
    EventChannel<InputEvent>::Subscription sub;

    UiEvents(World& world)
        : sub(make_subscription<InputEvent>(world))
    {
    }
};

static void trigger_events(World& world)
{
    UiEvents* events = nullptr;

    try {
        events = &world.get_or_emplace<UiEvents>(world);
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] " << e.what() << std::endl;
        return;
    }

    auto wininfo = world.get<WindowInfo>();

    if (!wininfo) {
        std::cerr << "[ERROR] No window info found!" << std::endl;
        return;
    }

    auto input_mgr = world.get<InputManager<>>();

    if (!input_mgr) {
        std::cerr << "[ERROR] No input manager found!" << std::endl;
        return;
    }

    auto entities = world.query<EventTarget, RectTransform>();

    vec2 mouse_pos = input_mgr->mouse().get_position();

    while (auto event = events->sub.next_event()) {
        if (event->type != InputEventType::MOUSE) {
            // next event!
            continue;
        }

        for (auto [ent, target, rect] : entities) {
            vec2 min = rect.abs_bounds_min();
            vec2 max = rect.abs_bounds_max();

            // input event have the origin in the top left corner
            min.y = static_cast<float>(wininfo->height) - min.y;
            max.y = static_cast<float>(wininfo->height) - max.y;
            std::swap(min.y, max.y);

            bool is_inside = mouse_pos.x >= min.x && mouse_pos.y >= min.y
                && mouse_pos.x <= max.x && mouse_pos.y <= max.y;

            // make sure the cursor is in the rectangle
            if (!is_inside) {
                if (world.get_component<MouseMovement>(ent)) {
                    using ige::plugin::ui::event::MouseLeave;

                    MouseLeave evt;
                    evt.absolute_pos = mouse_pos;
                    evt.pos = mouse_pos - min;
                    target.trigger<MouseLeave>(world, ent, evt);

                    world.remove_component<MouseMovement>(ent);
                }

                // next entity!
                continue;
            }

            auto& movement
                = world.get_or_emplace_component<MouseMovement>(ent, mouse_pos);

            using ige::plugin::ui::event::MouseClick;
            using ige::plugin::ui::event::MouseDown;
            using ige::plugin::ui::event::MouseEnter;
            using ige::plugin::ui::event::MouseMove;
            using ige::plugin::ui::event::MouseScroll;
            using ige::plugin::ui::event::MouseUp;

            switch (event->mouse.type) {
            case MouseEventType::BUTTON: {
                if (event->mouse.button.state == InputRegistryState::PRESSED) {
                    MouseDown evt;
                    evt.button = event->mouse.button.button;
                    evt.absolute_pos = mouse_pos;
                    evt.pos = mouse_pos - min;

                    target.trigger<MouseDown>(world, ent, evt);

                    movement.down = true;
                } else {
                    MouseUp evt;
                    evt.button = event->mouse.button.button;
                    evt.absolute_pos = mouse_pos;
                    evt.pos = mouse_pos - min;

                    target.trigger<MouseUp>(world, ent, evt);

                    if (movement.down) {
                        MouseClick click_evt;
                        click_evt.button = evt.button;
                        click_evt.absolute_pos = evt.absolute_pos;
                        click_evt.pos = evt.pos;

                        target.trigger<MouseClick>(world, ent, click_evt);
                    }
                }
            } break;
            case MouseEventType::MOUSE_MOVE: {
                if (!movement.entered) {
                    MouseEnter evt;
                    evt.absolute_pos = mouse_pos;
                    evt.pos = mouse_pos - min;

                    target.trigger<MouseEnter>(world, ent, evt);
                    movement.entered = true;
                }

                MouseMove evt;
                evt.absolute_pos = mouse_pos;
                evt.pos = mouse_pos - min;

                target.trigger<MouseMove>(world, ent, evt);
            } break;
            case MouseEventType::SCROLL: {
                MouseScroll evt;
                evt.delta.x = event->mouse.scroll.x;
                evt.delta.y = event->mouse.scroll.y;
                evt.absolute_pos = mouse_pos;
                evt.pos = mouse_pos - min;

                target.trigger<MouseScroll>(world, ent, evt);
            } break;
            }
        }
    }
}

void UiPlugin::plug(App::Builder& builder) const
{
    builder.add_system(System::from(trigger_events));
    builder.add_cleanup_system(
        System::from([](World& world) { world.remove<UiEvents>(); }));
}
