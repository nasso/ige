#include "ige.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::MouseButton;
using ige::plugin::input::MouseEventType;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

static std::unordered_map<InputRegistryState, std::string> STATE_TO_STRING = {
    { InputRegistryState::PRESSED, "PRESSED" },
    { InputRegistryState::RELEASED, "RELEASED" },
};

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;
    std::optional<EventChannel<InputEvent>::Subscription> m_input_events;

    void on_start(App& app) override
    {
        auto win_channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(win_channel->subscribe());

        auto input_channel = app.world().get<EventChannel<InputEvent>>();
        m_input_events.emplace(input_channel->subscribe());
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }

        if (auto manager = app.world().get<InputManager<>>()) {
            if (manager->mouse().is_pressed(MouseButton::LEFT)) {
                std::cout << "You pressed the left button of your mouse."
                          << std::endl;
            }

            auto scroll = manager->mouse().get_scroll();

            if (glm::length(scroll) >= 0.1f) {
                std::cout << "Scrolling (" << scroll.x << ", " << scroll.y
                          << ")" << std::endl;
            }

            auto movement = manager->mouse().get_movement();

            if (glm::length(movement) >= 0.1f) {
                std::cout << "Moving (" << movement.x << ", " << movement.y
                          << ")" << std::endl;
            }
        }

        while (auto event = m_input_events->next_event()) {
            if (event->type == InputEventType::MOUSE) {
                const auto& mouse_event = event->mouse;

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
        .add_plugin(InputPlugin<> {})
        .add_plugin(WindowPlugin {})
        .run<RootState>();

    std::cout << "Bye bye!" << std::endl;
}
