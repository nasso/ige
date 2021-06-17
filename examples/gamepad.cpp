#include "ige.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::input::ControllerAxis;
using ige::plugin::input::ControllerButton;
using ige::plugin::input::ControllerEventType;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::InputRegistryState;
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
            auto controllers = manager->controllers();
            for (auto controller_iter = controllers.first;
                 controller_iter != controllers.second; controller_iter++) {
                if (controller_iter->second.is_pressed(ControllerButton::X)) {
                    std::cout << "You pressed X." << std::endl;
                }

                std::cout << "Gamepad(" << controller_iter->second.id()
                          << ") : Left X axis = "
                          << controller_iter->second.get_axis_value(
                                 ControllerAxis::LEFT_X)
                          << std::endl;
                std::cout << "Gamepad(" << controller_iter->second.id()
                          << ") : Left Y axis = "
                          << controller_iter->second.get_axis_value(
                                 ControllerAxis::LEFT_Y)
                          << std::endl;
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
