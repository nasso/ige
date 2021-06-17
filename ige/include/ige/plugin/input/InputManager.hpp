#ifndef B7EA021F_3728_4158_AD5D_6BB02383B0D5
#define B7EA021F_3728_4158_AD5D_6BB02383B0D5

#include "Bindings.hpp"
#include "Controller.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "StringBindings.hpp"
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

namespace ige::plugin::input {

enum class InputEventType {
    KEYBOARD,
    MOUSE,
};

struct InputEvent {
    InputEventType type;
    union {
        KeyboardEvent keyboard;
        MouseEvent mouse;
    };
};

template <typename AxisId = std::string, typename ActionId = std::string>
class InputManager {
public:
    using ControllerMap = std::unordered_map<ControllerId, Controller>;
    using ControllerMapIterator = ControllerMap::iterator;
    using ControllerMapConstIterator = ControllerMap::const_iterator;

    std::optional<Bindings<AxisId, ActionId>> bindings;

    Keyboard& keyboard()
    {
        return m_keyboard;
    }

    const Keyboard& keyboard() const
    {
        return m_keyboard;
    }

    Mouse& mouse()
    {
        return m_mouse;
    }

    const Mouse& mouse() const
    {
        return m_mouse;
    }

    Controller& add_controller(const ControllerId& id)
    {
        return m_controllers.emplace(id, id).first->second;
    }

    std::pair<ControllerMapConstIterator, ControllerMapConstIterator>
    controllers() const
    {
        return { m_controllers.begin(), m_controllers.end() };
    }

    std::pair<ControllerMapIterator, ControllerMapIterator> controllers();
    {
        return { m_controllers.begin(), m_controllers.end() };
    }

    Controller* controller(const ControllerId& id)
    {
        auto controller = m_controllers.find(id);

        if (controller == m_controllers.end()) {
            return nullptr;
        }
        return &controller->second;
    }

    const Controller* controller(const ControllerId& id) const
    {
        auto controller = m_controllers.find(id);

        if (controller == m_controllers.end()) {
            return nullptr;
        }
        return &controller->second;
    }

    void reset()
    {
        m_keyboard.clear();
        m_mouse.clear();
        for (auto& [controller_id, controller] : m_controllers) {
            controller.clear();
        }
    }

    void push_event(const InputEvent& event)
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

    bool is_button_down(const Button& button) const
    {
        if (std::holds_alternative<KeyboardKey>(button)) {
            return m_keyboard.is_down(std::get<KeyboardKey>(button));
        } else if (std::holds_alternative<
                       std::pair<ControllerId, ControllerButton>>(button)) {
            auto bt
                = std::get<std::pair<ControllerId, ControllerButton>>(button);
            auto controller = m_controllers.find(bt.first);

            if (controller == m_controllers.end()) {
                return false;
            }
            return controller->second.is_down(bt.second);
        } else if (std::holds_alternative<MouseButton>(button)) {
            return m_mouse.is_down(std::get<MouseButton>(button));
        }
        return false;
    }

    std::optional<bool> is_action_down(const ActionId& id) const
    {
        if (!bindings) {
            return {};
        }

        if (auto action = bindings->action(id)) {
            if (std::holds_alternative<Button>(*action)) {
                return is_button_down(std::get<Button>(*action));
            } else if (std::holds_alternative<std::vector<Button>>(*action)) {
                for (const auto& button :
                     std::get<std::vector<Button>>(*action)) {
                    if (!is_button_down(button)) {
                        return false;
                    }
                }
                return true;
            }
        }

        return {};
    }

    std::optional<float> get_axis_value(const AxisId& id) const
    {
        if (!bindings) {
            return {};
        }

        if (auto axis = bindings->axis(id)) {
            if (std::holds_alternative<BindingAxisEmulated>(*axis)) {
                auto a = std::get<BindingAxisEmulated>(*axis);
                bool pos = is_button_down(a.pos);
                bool neg = is_button_down(a.neg);

                if (pos == neg) {
                    return 0.0f;
                } else {
                    return pos ? 1.0f : -1.0f;
                }

            } else if (std::holds_alternative<BindingAxisController>(*axis)) {
                auto a = std::get<BindingAxisController>(*axis);
                auto controller = m_controllers.find(a.controller_id);

                if (controller == m_controllers.end()) {
                    return 0.0f;
                }

                auto val = controller->second.get_axis_value(a.axis);

                if (val < -a.dead_zone) {
                    return (val + a.dead_zone) / (1.0 - a.dead_zone);
                } else if (val > a.dead_zone) {
                    return (val - a.dead_zone) / (1.0 - a.dead_zone);
                } else {
                    return 0.0f;
                }
            }
        }

        return {};
    }

private:
    std::unordered_map<ControllerId, Controller> m_controllers;
    Keyboard m_keyboard;
    Mouse m_mouse;
};

}

#endif /* B7EA021F_3728_4158_AD5D_6BB02383B0D5 */
