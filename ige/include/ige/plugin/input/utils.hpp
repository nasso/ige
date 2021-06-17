#ifndef BEFDDA56_916C_41CB_83B0_6F35001E0074
#define BEFDDA56_916C_41CB_83B0_6F35001E0074

#include "Controller.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include <optional>
#include <string>

namespace ige::plugin::input {
class InputsFromString {
public:
    static std::optional<KeyboardKey>
    keyboard_key_from_string(const std::string& str);
    static std::optional<ControllerButton>
    controller_button_from_string(const std::string& str);
    static std::optional<ControllerAxis>
    controller_axis_from_string(const std::string& str);
    static std::optional<MouseButton>
    mouse_button_from_string(const std::string& str);
};

class InputsToString {
public:
    static std::string keyboard_key_to_string(const KeyboardKey& key);
    static std::string
    controller_button_to_string(const ControllerButton& button);
    static std::string controller_axis_to_string(const ControllerAxis& axis);
    static std::string mouse_button_to_string(const MouseButton& button);
};

}

#endif /* BEFDDA56_916C_41CB_83B0_6F35001E0074 */
