#include "ige.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::InputPlugin;
using ige::plugin::WindowEvent;
using ige::plugin::WindowEventKind;
using ige::plugin::WindowingPlugin;
using ige::plugin::WindowSettings;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::MouseButton;
using ige::plugin::input::MouseEventType;

static std::unordered_map<InputRegistryState, std::string> STATE_TO_STRING = {
    { InputRegistryState::DOWN, "DOWN" },
    { InputRegistryState::PRESSED, "PRESSED" },
    { InputRegistryState::RELEASED, "RELEASED" },
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<EventChannel<InputEvent>::Subscription> m_input_events;

    void on_start(App& app) override
    {
        auto win_channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(win_channel->get().subscribe());
        auto input_channel = app.world().get<EventChannel<InputEvent>>();
        m_input_events.emplace(input_channel->get().subscribe());
    }

    void on_update(App& app) override
    {
        while (const auto& event = m_win_events->next_event()) {
            if (event->get().kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }

        const auto opt_manager = app.world().get<InputManager>();

        if (opt_manager) {
            const auto& manager = opt_manager->get();

            if (manager.mouse().is_pressed(MouseButton::LEFT)) {
                std::cout << "You pressed the left button of your mouse."
                          << std::endl;
            }
        }

        while (const auto& opt_event = m_input_events->next_event()) {
            const auto& event = opt_event->get();
            if (event.type == InputEventType::MOUSE) {
                auto& mouse_event = event.mouse;
                if (mouse_event.type == MouseEventType::BUTTON) {
                    std::cout << "Mouse button: "
                              << static_cast<int>(mouse_event.button.button)
                              << ", Value :"
                              << STATE_TO_STRING[mouse_event.button.state]
                              << std::endl;
                } else if (mouse_event.type == MouseEventType::MOUSE_MOVE) {
                    std::cout << "Mouse position (x: " << mouse_event.pos.x
                              << ", y: " << mouse_event.pos.y << ")"
                              << std::endl;
                }
            }
        }
    }
};

int main()
{
    std::cout << "Starting application..." << std::endl;

    App::Builder()
        .insert(WindowSettings { "Hello, World!", 800, 600 })
        .add_plugin(InputPlugin {})
        .add_plugin(WindowingPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
