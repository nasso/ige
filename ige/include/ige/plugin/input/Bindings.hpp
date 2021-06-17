#ifndef DF5E2FB2_426C_4E1B_A78E_CBE9330456CB
#define DF5E2FB2_426C_4E1B_A78E_CBE9330456CB

#include "Controller.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "utils.hpp"
#include <fstream>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <utility>
#include <variant>

namespace ige::plugin::input {
using Button = std::variant<
    KeyboardKey, std::pair<ControllerId, ControllerButton>, MouseButton>;

template <typename T>
class BindingIdentifier {
public:
    std::optional<T> from_json(const nlohmann::json& src);
    nlohmann::json to_json(const T& src);
};

struct BindingAxisEmulated {
    Button pos;
    Button neg;
};

struct BindingAxisController {
    ControllerId controller_id;
    ControllerAxis axis;
    float dead_zone;
};

using BindingAxis = std::variant<BindingAxisEmulated, BindingAxisController>;

using BindingAction = std::variant<Button, std::vector<Button>>;

template <typename AxisId = std::string, typename ActionId = std::string>
class Bindings {
public:
    using AxisMap = std::unordered_map<AxisId, BindingAxis>;
    using AxisMapIter = AxisMap::iterator;
    using ActionsMap = std::unordered_map<ActionId, BindingAction>;
    using ActionsMapIter = ActionsMap::iterator;

    static std::optional<Bindings> from_file(const std::string& path)
    {
        try {
            Bindings bindings;
            std::ifstream i(path);
            nlohmann::json json = nlohmann::json::parse(i);

            if (json.find("axes") != json.end() && json["axes"].is_array()) {
                auto axes = json["axes"];
                for (const auto& axis : axes) {
                    if (axis.find("id") == axis.end()
                        || axis.find("value") == axis.end()) {
                        continue;
                    }
                    auto parsed_axis = parse_axis(axis["value"]);
                    auto parsed_id
                        = BindingIdentifier<AxisId> {}.from_json(axis["id"]);
                    if (parsed_axis && parsed_id) {
                        bindings.m_axes.emplace(*parsed_id, *parsed_axis);
                    }
                }
            }

            if (json.find("axes") != json.end() && json["actions"].is_array()) {
                auto actions = json["actions"];

                for (const auto& action : actions) {
                    if (action.find("id") == action.end()
                        || action.find("value") == action.end()) {
                        continue;
                    }
                    auto parsed_action = parse_action(action["value"]);
                    auto parsed_id = BindingIdentifier<ActionId> {}.from_json(
                        action["id"]);
                    if (parsed_action && parsed_id) {
                        bindings.m_actions.emplace(*parsed_id, *parsed_action);
                    }
                }
            }
            return bindings;
        } catch (...) {
            return {};
        }
    }

    void to_file(const std::string& path)
    {
        nlohmann::json json;
        std::ofstream o(path);

        json["axes"] = nlohmann::json::array();
        for (const auto& axis : m_axes) {
            nlohmann::json axis_json;
            axis_json["id"] = BindingIdentifier<AxisId> {}.to_json(axis.first);
            axis_json["value"] = serialize_axis(axis.second);
            json["axes"].push_back(axis_json);
        }

        json["actions"] = nlohmann::json::array();

        for (const auto& action : m_actions) {
            nlohmann::json action_json;
            action_json["id"]
                = BindingIdentifier<AxisId> {}.to_json(action.first);
            action_json["value"] = serialize_action(action.second);

            json["actions"].push_back(action_json);
        }

        o << json;
    }

    void insert_axis(const AxisId& id, const BindingAxis& axis)
    {
        m_axes.emplace(id, axis);
    }

    void remove_axis(const AxisId& id)
    {
        m_axes.erase(id);
    }

    BindingAxis* axis(const AxisId& id)
    {
        auto axis = m_axes.find(id);

        if (axis == m_axes.end()) {
            return nullptr;
        }
        return &axis->second;
    }

    const BindingAxis* axis(const AxisId& id) const
    {
        auto axis = m_axes.find(id);

        if (axis == m_axes.end()) {
            return nullptr;
        }
        return &axis->second;
    }

    const AxisMapIter axes() const
    {
        return m_axes.begin();
    }

    AxisMapIter axes()
    {
        return m_axes.begin();
    }

    void insert_action(const ActionId& id, const BindingAction& action)
    {
        m_actions.emplace(id, action);
    }

    void remove_action(const ActionId& id)
    {
        m_actions.erase(id);
    }

    BindingAction* action(const ActionId& id)
    {
        auto action = m_actions.find(id);

        if (action == m_actions.end()) {
            return nullptr;
        }
        return &action->second;
    }

    const BindingAction* action(const ActionId& id) const
    {
        auto action = m_actions.find(id);

        if (action == m_actions.end()) {
            return nullptr;
        }
        return &action->second;
    }

    const ActionsMapIter actions() const
    {
        return m_actions.begin();
    }

    ActionsMapIter actions()
    {
        return m_actions.begin();
    }

private:
    static nlohmann::json serilalize_button(const Button& button)
    {
        nlohmann::json json;

        if (std::holds_alternative<KeyboardKey>(button)) {
            json["type"] = "Keyboard";
            json["key"] = InputsToString::keyboard_key_to_string(
                std::get<KeyboardKey>(button));
        } else if (std::holds_alternative<
                       std::pair<ControllerId, ControllerButton>>(button)) {
            auto b
                = std::get<std::pair<ControllerId, ControllerButton>>(button);
            json["type"] = "Controller";
            json["controller_id"] = b.first;
            json["button"]
                = InputsToString::controller_button_to_string(b.second);
        } else if (std::holds_alternative<MouseButton>(button)) {
            json["type"] = "Mouse";
            json["button"] = InputsToString::mouse_button_to_string(
                std::get<MouseButton>(button));
        }

        return json;
    }

    static std::optional<Button> parse_button(const nlohmann::json& src)
    {
        if (src.find("type") != src.end() && src["type"].is_string()) {
            auto type = src["type"].get<std::string>();
            Button button;

            if (type == "Keyboard" && src.find("key") != src.end()
                && src["key"].is_string()) {
                if (auto k = InputsFromString::keyboard_key_from_string(
                        src["key"].get<std::string>())) {
                    button = *k;
                    return button;
                } else {
                    return {};
                }
            } else if (
                type == "Controller" && src.find("button") != src.end()
                && src["button"].is_string()
                && src.find("controller_id") != src.end()
                && src["controller_id"].is_number()) {
                if (auto b = InputsFromString::controller_button_from_string(
                        src["button"].get<std::string>())) {
                    button = std::pair<ControllerId, ControllerButton> {
                        src["controller_id"].get<ControllerId>(), *b
                    };
                    return button;
                } else {
                    return {};
                }
            } else if (
                type == "Mouse" && src.find("button") != src.end()
                && src["button"].is_string()) {
                if (auto b = InputsFromString::mouse_button_from_string(
                        src["button"].get<std::string>())) {
                    button = *b;
                    return button;
                } else {
                    return {};
                }
            }
        }
        return {};
    }

    static nlohmann::json serialize_axis(const BindingAxis& axis)
    {
        nlohmann::json json;

        if (std::holds_alternative<BindingAxisEmulated>(axis)) {
            auto a = std::get<BindingAxisEmulated>(axis);
            json["pos"] = serilalize_button(a.neg);
            json["neg"] = serilalize_button(a.pos);
        } else if (std::holds_alternative<BindingAxisController>(axis)) {
            auto a = std::get<BindingAxisController>(axis);
            json["controller_id"] = a.controller_id;
            json["axis"] = InputsToString::controller_axis_to_string(a.axis);
            json["dead_zone"] = a.dead_zone;
        }
        return json;
    }

    static std::optional<BindingAxis> parse_axis(const nlohmann::json& src)
    {
        if (src.find("pos") != src.end() && src["pos"].is_object()
            && src.find("neg") != src.end() && src["neg"].is_object()) {
            BindingAxisEmulated axis;
            auto pos = parse_button(src["pos"]);
            auto neg = parse_button(src["neg"]);

            if (!pos || !neg) {
                return {};
            }
            axis.pos = *pos;
            axis.neg = *neg;
            return axis;
        } else if (
            src.find("controller_id") != src.end()
            && src["controller_id"].is_number() && src.find("axis") != src.end()
            && src["axis"].is_string()) {
            BindingAxisController axis;
            axis.controller_id = src["controller_id"].get<ControllerId>();
            auto controller_axis
                = InputsFromString::controller_axis_from_string(
                    src["axis"].get<std::string>());

            if (!controller_axis) {
                return {};
            }
            axis.axis = *controller_axis;
            axis.dead_zone = 0.0f;
            if (src.find("dead_zone") != src.end()
                && src["dead_zone"].is_number_float()) {
                axis.dead_zone = src["dead_zone"].get<float>();
            }
            return axis;
        }
        return {};
    }

    static nlohmann::json serialize_action(const BindingAction& action)
    {
        nlohmann::json json;

        if (std::holds_alternative<Button>(action)) {
            json = serilalize_button(std::get<Button>(action));
        } else if (std::holds_alternative<std::vector<Button>>(action)) {
            json = nlohmann::json::array();
            for (const auto& button : std::get<std::vector<Button>>(action)) {
                json.push_back(serilalize_button(button));
            }
        }
        return json;
    }

    static std::optional<BindingAction> parse_action(const nlohmann::json& src)
    {
        if (src.is_object()) {
            return parse_button(src);
        } else if (src.is_array() && src.size() != 0) {
            std::vector<Button> buttons;
            for (auto& elem : src) {
                auto button = parse_button(elem);
                if (!button) {
                    return {};
                }
                buttons.push_back(*button);
            }
            return buttons;
        }
        return {};
    }

    AxisMap m_axes;
    ActionsMap m_actions;
};

}

#endif /* DF5E2FB2_426C_4E1B_A78E_CBE9330456CB */
