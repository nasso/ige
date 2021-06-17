#include "ige.hpp"
#include <iostream>
#include <optional>
#include <unordered_map>

using ige::core::App;
using ige::core::EventChannel;
using ige::core::State;
using ige::ecs::Schedule;
using ige::plugin::input::Bindings;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputPlugin;
using ige::plugin::input::KeyboardKey;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

class RootState : public State {
    std::optional<EventChannel<WindowEvent>::Subscription> m_win_events;

    void on_start(App& app) override
    {
        auto win_channel = app.world().get<EventChannel<WindowEvent>>();
        m_win_events.emplace(win_channel->subscribe());

        if (auto manager = app.world().get<InputManager<>>()) {
            manager->bindings = Bindings<>::from_file("./config/bindings.json");
        }
    }

    void on_update(App& app) override
    {
        while (auto event = m_win_events->next_event()) {
            if (event->kind == WindowEventKind::WindowClose) {
                app.quit();
            }
        }

        if (auto manager = app.world().get<InputManager<>>()) {
            auto action = manager->is_action_down("press_y");
            if (action && *action) {
                std::cout << "Action detected when you pressed Y" << std::endl;
            }

            auto axis = manager->get_axis_value("key_axis");
            if (axis) {
                std::cout << *axis << std::endl;
            }
        }
    }

    void on_stop(App& app) override
    {
        std::cout << "Called" << std::endl;

        if (auto manager = app.world().get<InputManager<>>()) {
            manager->bindings->to_file("./config/new_bindings.json");
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
