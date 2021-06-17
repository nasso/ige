#include "ige/plugin/input/utils.hpp"
#include "ige/plugin/input/Controller.hpp"
#include "ige/plugin/input/Keyboard.hpp"
#include "ige/plugin/input/Mouse.hpp"
#include <string>

using ige::plugin::input::ControllerAxis;
using ige::plugin::input::ControllerButton;
using ige::plugin::input::InputsFromString;
using ige::plugin::input::InputsToString;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;

static std::unordered_map<ControllerAxis, std::string> CONTROLLER_AXIS_TO_STRING
    = {
          { ControllerAxis::LEFT_X, "LEFT_X" },
          { ControllerAxis::LEFT_Y, "LEFT_Y" },
          { ControllerAxis::RIGHT_X, "RIGHT_X" },
          { ControllerAxis::RIGHT_Y, "RIGHT_Y" },
          { ControllerAxis::LEFT_TRIGGER, "LEFT_TRIGGER" },
          { ControllerAxis::RIGHT_TRIGGER, "RIGHT_TRIGGER" },
      };

static std::unordered_map<std::string, ControllerAxis> STRING_TO_CONTROLLER_AXIS
    = {
          { "LEFT_X", ControllerAxis::LEFT_X },
          { "LEFT_Y", ControllerAxis::LEFT_Y },
          { "RIGHT_X", ControllerAxis::RIGHT_X },
          { "RIGHT_Y", ControllerAxis::RIGHT_Y },
          { "LEFT_TRIGGER", ControllerAxis::LEFT_TRIGGER },
          { "RIGHT_TRIGGER", ControllerAxis::RIGHT_TRIGGER },
      };

static std::unordered_map<ControllerButton, std::string>
    CONTROLLER_BUTTON_TO_STRING = {
        { ControllerButton::A, "A" },
        { ControllerButton::B, "B" },
        { ControllerButton::X, "X" },
        { ControllerButton::Y, "Y" },
        { ControllerButton::DPAD_DOWN, "DPAD_DOWN" },
        { ControllerButton::DPAD_LEFT, "DPAD_LEFT" },
        { ControllerButton::DPAD_RIGHT, "DPAD_RIGHT" },
        { ControllerButton::DPAD_UP, "DPAD_UP" },
        { ControllerButton::LEFT_BUMPER, "LEFT_BUMPER" },
        { ControllerButton::RIGHT_BUMPER, "RIGHT_BUMPER" },
        { ControllerButton::LEFT_THUMB, "LEFT_THUMB" },
        { ControllerButton::RIGHT_THUMB, "RIGHT_THUMB" },
        { ControllerButton::BACK, "BACK" },
        { ControllerButton::START, "START" },
        { ControllerButton::GUIDE, "GUIDE" },
    };

static std::unordered_map<std::string, ControllerButton>
    STRING_TO_CONTROLLER_BUTTON = {
        { "A", ControllerButton::A },
        { "B", ControllerButton::B },
        { "X", ControllerButton::X },
        { "Y", ControllerButton::Y },
        { "DPAD_DOWN", ControllerButton::DPAD_DOWN },
        { "DPAD_LEFT", ControllerButton::DPAD_LEFT },
        { "DPAD_RIGHT", ControllerButton::DPAD_RIGHT },
        { "DPAD_UP", ControllerButton::DPAD_UP },
        { "LEFT_BUMPER", ControllerButton::LEFT_BUMPER },
        { "RIGHT_BUMPER", ControllerButton::RIGHT_BUMPER },
        { "LEFT_THUMB", ControllerButton::LEFT_THUMB },
        { "RIGHT_THUMB", ControllerButton::RIGHT_THUMB },
        { "BACK", ControllerButton::BACK },
        { "START", ControllerButton::START },
        { "GUIDE", ControllerButton::GUIDE },
    };

static std::unordered_map<std::string, KeyboardKey> STRING_TO_KEYBOARD_KEY = {
    { "KEY_0", KeyboardKey::KEY_0 },
    { "KEY_1", KeyboardKey::KEY_1 },
    { "KEY_2", KeyboardKey::KEY_2 },
    { "KEY_3", KeyboardKey::KEY_3 },
    { "KEY_4", KeyboardKey::KEY_4 },
    { "KEY_5", KeyboardKey::KEY_5 },
    { "KEY_6", KeyboardKey::KEY_6 },
    { "KEY_7", KeyboardKey::KEY_7 },
    { "KEY_8", KeyboardKey::KEY_8 },
    { "KEY_9", KeyboardKey::KEY_9 },
    { "KEY_A", KeyboardKey::KEY_A },
    { "KEY_ALT_LEFT", KeyboardKey::KEY_ALT_LEFT },
    { "KEY_ALT_RIGHT", KeyboardKey::KEY_ALT_RIGHT },
    { "KEY_ARROW_DOWN", KeyboardKey::KEY_ARROW_DOWN },
    { "KEY_ARROW_LEFT", KeyboardKey::KEY_ARROW_LEFT },
    { "KEY_ARROW_RIGHT", KeyboardKey::KEY_ARROW_RIGHT },
    { "KEY_ARROW_UP", KeyboardKey::KEY_ARROW_UP },
    { "KEY_B", KeyboardKey::KEY_B },
    { "KEY_BACK_SPACE", KeyboardKey::KEY_BACK_SPACE },
    { "KEY_C", KeyboardKey::KEY_C },
    { "KEY_CTRL_LEFT", KeyboardKey::KEY_CTRL_LEFT },
    { "KEY_CTRL_RIGHT", KeyboardKey::KEY_CTRL_RIGHT },
    { "KEY_D", KeyboardKey::KEY_D },
    { "KEY_DELETE", KeyboardKey::KEY_DELETE },
    { "KEY_E", KeyboardKey::KEY_E },
    { "KEY_END_FILE", KeyboardKey::KEY_END_FILE },
    { "KEY_ESCAPE", KeyboardKey::KEY_ESCAPE },
    { "KEY_F", KeyboardKey::KEY_F },
    { "KEY_F1", KeyboardKey::KEY_F1 },
    { "KEY_F10", KeyboardKey::KEY_F10 },
    { "KEY_F11", KeyboardKey::KEY_F11 },
    { "KEY_F12", KeyboardKey::KEY_F12 },
    { "KEY_F2", KeyboardKey::KEY_F2 },
    { "KEY_F3", KeyboardKey::KEY_F3 },
    { "KEY_F4", KeyboardKey::KEY_F4 },
    { "KEY_F5", KeyboardKey::KEY_F5 },
    { "KEY_F6", KeyboardKey::KEY_F6 },
    { "KEY_F7", KeyboardKey::KEY_F7 },
    { "KEY_F8", KeyboardKey::KEY_F8 },
    { "KEY_F9", KeyboardKey::KEY_F9 },
    { "KEY_G", KeyboardKey::KEY_G },
    { "KEY_H", KeyboardKey::KEY_H },
    { "KEY_I", KeyboardKey::KEY_I },
    { "KEY_INSERT", KeyboardKey::KEY_INSERT },
    { "KEY_J", KeyboardKey::KEY_J },
    { "KEY_K", KeyboardKey::KEY_K },
    { "KEY_L", KeyboardKey::KEY_L },
    { "KEY_M", KeyboardKey::KEY_M },
    { "KEY_N", KeyboardKey::KEY_N },
    { "KEY_O", KeyboardKey::KEY_O },
    { "KEY_P", KeyboardKey::KEY_P },
    { "KEY_PAGE_DOWN", KeyboardKey::KEY_PAGE_DOWN },
    { "KEY_PAGE_UP", KeyboardKey::KEY_PAGE_UP },
    { "KEY_Q", KeyboardKey::KEY_Q },
    { "KEY_R", KeyboardKey::KEY_R },
    { "KEY_S", KeyboardKey::KEY_S },
    { "KEY_SHIFT_LEFT", KeyboardKey::KEY_SHIFT_LEFT },
    { "KEY_SHIFT_RIGHT", KeyboardKey::KEY_SHIFT_RIGHT },
    { "KEY_SPACE", KeyboardKey::KEY_SPACE },
    { "KEY_T", KeyboardKey::KEY_T },
    { "KEY_TAB", KeyboardKey::KEY_TAB },
    { "KEY_U", KeyboardKey::KEY_U },
    { "KEY_V", KeyboardKey::KEY_V },
    { "KEY_W", KeyboardKey::KEY_W },
    { "KEY_X", KeyboardKey::KEY_X },
    { "KEY_Y", KeyboardKey::KEY_Y },
    { "KEY_Z", KeyboardKey::KEY_Z },
};

static std::unordered_map<KeyboardKey, std::string> KEYBOARD_KEY_TO_STRING = {
    { KeyboardKey::KEY_0, "KEY_0" },
    { KeyboardKey::KEY_1, "KEY_1" },
    { KeyboardKey::KEY_2, "KEY_2" },
    { KeyboardKey::KEY_3, "KEY_3" },
    { KeyboardKey::KEY_4, "KEY_4" },
    { KeyboardKey::KEY_5, "KEY_5" },
    { KeyboardKey::KEY_6, "KEY_6" },
    { KeyboardKey::KEY_7, "KEY_7" },
    { KeyboardKey::KEY_8, "KEY_8" },
    { KeyboardKey::KEY_9, "KEY_9" },
    { KeyboardKey::KEY_A, "KEY_A" },
    { KeyboardKey::KEY_ALT_LEFT, "KEY_ALT_LEFT" },
    { KeyboardKey::KEY_ALT_RIGHT, "KEY_ALT_RIGHT" },
    { KeyboardKey::KEY_ARROW_DOWN, "KEY_ARROW_DOWN" },
    { KeyboardKey::KEY_ARROW_LEFT, "KEY_ARROW_LEFT" },
    { KeyboardKey::KEY_ARROW_RIGHT, "KEY_ARROW_RIGHT" },
    { KeyboardKey::KEY_ARROW_UP, "KEY_ARROW_UP" },
    { KeyboardKey::KEY_B, "KEY_B" },
    { KeyboardKey::KEY_BACK_SPACE, "KEY_BACK_SPACE" },
    { KeyboardKey::KEY_C, "KEY_C" },
    { KeyboardKey::KEY_CTRL_LEFT, "KEY_CTRL_LEFT" },
    { KeyboardKey::KEY_CTRL_RIGHT, "KEY_CTRL_RIGHT" },
    { KeyboardKey::KEY_D, "KEY_D" },
    { KeyboardKey::KEY_DELETE, "KEY_DELETE" },
    { KeyboardKey::KEY_E, "KEY_E" },
    { KeyboardKey::KEY_END_FILE, "KEY_END_FILE" },
    { KeyboardKey::KEY_ESCAPE, "KEY_ESCAPE" },
    { KeyboardKey::KEY_F, "KEY_F" },
    { KeyboardKey::KEY_F1, "KEY_F1" },
    { KeyboardKey::KEY_F10, "KEY_F10" },
    { KeyboardKey::KEY_F11, "KEY_F11" },
    { KeyboardKey::KEY_F12, "KEY_F12" },
    { KeyboardKey::KEY_F2, "KEY_F2" },
    { KeyboardKey::KEY_F3, "KEY_F3" },
    { KeyboardKey::KEY_F4, "KEY_F4" },
    { KeyboardKey::KEY_F5, "KEY_F5" },
    { KeyboardKey::KEY_F6, "KEY_F6" },
    { KeyboardKey::KEY_F7, "KEY_F7" },
    { KeyboardKey::KEY_F8, "KEY_F8" },
    { KeyboardKey::KEY_F9, "KEY_F9" },
    { KeyboardKey::KEY_G, "KEY_G" },
    { KeyboardKey::KEY_H, "KEY_H" },
    { KeyboardKey::KEY_I, "KEY_I" },
    { KeyboardKey::KEY_INSERT, "KEY_INSERT" },
    { KeyboardKey::KEY_J, "KEY_J" },
    { KeyboardKey::KEY_K, "KEY_K" },
    { KeyboardKey::KEY_L, "KEY_L" },
    { KeyboardKey::KEY_M, "KEY_M" },
    { KeyboardKey::KEY_N, "KEY_N" },
    { KeyboardKey::KEY_O, "KEY_O" },
    { KeyboardKey::KEY_P, "KEY_P" },
    { KeyboardKey::KEY_PAGE_DOWN, "KEY_PAGE_DOWN" },
    { KeyboardKey::KEY_PAGE_UP, "KEY_PAGE_UP" },
    { KeyboardKey::KEY_Q, "KEY_Q" },
    { KeyboardKey::KEY_R, "KEY_R" },
    { KeyboardKey::KEY_S, "KEY_S" },
    { KeyboardKey::KEY_SHIFT_LEFT, "KEY_SHIFT_LEFT" },
    { KeyboardKey::KEY_SHIFT_RIGHT, "KEY_SHIFT_RIGHT" },
    { KeyboardKey::KEY_SPACE, "KEY_SPACE" },
    { KeyboardKey::KEY_T, "KEY_T" },
    { KeyboardKey::KEY_TAB, "KEY_TAB" },
    { KeyboardKey::KEY_U, "KEY_U" },
    { KeyboardKey::KEY_V, "KEY_V" },
    { KeyboardKey::KEY_W, "KEY_W" },
    { KeyboardKey::KEY_X, "KEY_X" },
    { KeyboardKey::KEY_Y, "KEY_Y" },
    { KeyboardKey::KEY_Z, "KEY_Z" },
};

static std::unordered_map<MouseButton, std::string> MOUSE_BUTTON_TO_STRING = {
    { MouseButton::LEFT, "LEFT" },
    { MouseButton::RIGHT, "RIGHT" },
    { MouseButton::MIDDLE, "MIDDLE" },
};

static std::unordered_map<std::string, MouseButton> STRING_TO_MOUSE_BUTTON = {
    { "LEFT", MouseButton::LEFT },
    { "RIGHT", MouseButton::RIGHT },
    { "MIDDLE", MouseButton::MIDDLE },
};

std::optional<KeyboardKey>
InputsFromString::keyboard_key_from_string(const std::string& str)
{
    auto key = STRING_TO_KEYBOARD_KEY.find(str);

    if (key == STRING_TO_KEYBOARD_KEY.end()) {
        return {};
    }
    return key->second;
}

std::optional<ControllerButton>
InputsFromString::controller_button_from_string(const std::string& str)
{
    auto button = STRING_TO_CONTROLLER_BUTTON.find(str);

    if (button == STRING_TO_CONTROLLER_BUTTON.end()) {
        return {};
    }
    return button->second;
}

std::optional<MouseButton>
InputsFromString::mouse_button_from_string(const std::string& str)
{
    auto button = STRING_TO_MOUSE_BUTTON.find(str);

    if (button == STRING_TO_MOUSE_BUTTON.end()) {
        return {};
    }
    return button->second;
}

std::optional<ControllerAxis>
InputsFromString::controller_axis_from_string(const std::string& str)
{
    auto axis = STRING_TO_CONTROLLER_AXIS.find(str);

    if (axis == STRING_TO_CONTROLLER_AXIS.end()) {
        return {};
    }
    return axis->second;
}

std::string InputsToString::keyboard_key_to_string(const KeyboardKey& key)
{
    return KEYBOARD_KEY_TO_STRING[key];
}

std::string
InputsToString::controller_button_to_string(const ControllerButton& button)
{
    return CONTROLLER_BUTTON_TO_STRING[button];
}

std::string
InputsToString::controller_axis_to_string(const ControllerAxis& axis)
{
    return CONTROLLER_AXIS_TO_STRING[axis];
}

std::string InputsToString::mouse_button_to_string(const MouseButton& button)
{
    return MOUSE_BUTTON_TO_STRING[button];
}
