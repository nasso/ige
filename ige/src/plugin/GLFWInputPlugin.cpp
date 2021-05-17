#include "ige/plugin/GLFWInputPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/input/input.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using ige::core::App;
using ige::core::EventChannel;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::GLFWInputPlugin;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;
using ige::plugin::input::MouseEvent;
using ige::plugin::input::MouseEventType;

const std::unordered_map<int, InputRegistryState> GLFW_TO_REGISTRYSTATE = {
    { GLFW_PRESS, InputRegistryState::PRESSED },
    { GLFW_RELEASE, InputRegistryState::RELEASED },
    { GLFW_REPEAT, InputRegistryState::DOWN },
};

const std::unordered_map<int, KeyboardKey> GLFW_TO_KEYBOARD_KEYS = {
    { GLFW_KEY_A, KeyboardKey::KEY_A }, { GLFW_KEY_B, KeyboardKey::KEY_B },
    { GLFW_KEY_C, KeyboardKey::KEY_C }, { GLFW_KEY_D, KeyboardKey::KEY_D },
    { GLFW_KEY_E, KeyboardKey::KEY_E }, { GLFW_KEY_F, KeyboardKey::KEY_F },
    { GLFW_KEY_G, KeyboardKey::KEY_G }, { GLFW_KEY_H, KeyboardKey::KEY_H },
    { GLFW_KEY_I, KeyboardKey::KEY_I }, { GLFW_KEY_J, KeyboardKey::KEY_J },
    { GLFW_KEY_K, KeyboardKey::KEY_K }, { GLFW_KEY_L, KeyboardKey::KEY_L },
    { GLFW_KEY_M, KeyboardKey::KEY_M }, { GLFW_KEY_N, KeyboardKey::KEY_N },
    { GLFW_KEY_O, KeyboardKey::KEY_O }, { GLFW_KEY_P, KeyboardKey::KEY_P },
    { GLFW_KEY_Q, KeyboardKey::KEY_Q }, { GLFW_KEY_R, KeyboardKey::KEY_R },
    { GLFW_KEY_S, KeyboardKey::KEY_S }, { GLFW_KEY_T, KeyboardKey::KEY_T },
    { GLFW_KEY_U, KeyboardKey::KEY_U }, { GLFW_KEY_V, KeyboardKey::KEY_V },
    { GLFW_KEY_W, KeyboardKey::KEY_W }, { GLFW_KEY_X, KeyboardKey::KEY_X },
    { GLFW_KEY_Y, KeyboardKey::KEY_Y }, { GLFW_KEY_Z, KeyboardKey::KEY_Z },
    { GLFW_KEY_BACKSPACE, KeyboardKey::KEY_BACK_SPACE },
    { GLFW_KEY_TAB, KeyboardKey::KEY_TAB },
    { GLFW_KEY_ESCAPE, KeyboardKey::KEY_ESCAPE },
    { GLFW_KEY_DELETE, KeyboardKey::KEY_DELETE },
    { GLFW_KEY_INSERT, KeyboardKey::KEY_INSERT },
    { GLFW_KEY_0, KeyboardKey::KEY_0 }, { GLFW_KEY_1, KeyboardKey::KEY_1 },
    { GLFW_KEY_2, KeyboardKey::KEY_2 }, { GLFW_KEY_3, KeyboardKey::KEY_3 },
    { GLFW_KEY_4, KeyboardKey::KEY_4 }, { GLFW_KEY_5, KeyboardKey::KEY_5 },
    { GLFW_KEY_6, KeyboardKey::KEY_6 }, { GLFW_KEY_7, KeyboardKey::KEY_7 },
    { GLFW_KEY_8, KeyboardKey::KEY_8 }, { GLFW_KEY_9, KeyboardKey::KEY_9 },
    { GLFW_KEY_F1, KeyboardKey::KEY_F1 }, { GLFW_KEY_F2, KeyboardKey::KEY_F2 },
    { GLFW_KEY_F3, KeyboardKey::KEY_F3 }, { GLFW_KEY_F4, KeyboardKey::KEY_F4 },
    { GLFW_KEY_F5, KeyboardKey::KEY_F5 }, { GLFW_KEY_F6, KeyboardKey::KEY_F6 },
    { GLFW_KEY_F7, KeyboardKey::KEY_F7 }, { GLFW_KEY_F8, KeyboardKey::KEY_F8 },
    { GLFW_KEY_F9, KeyboardKey::KEY_F9 },
    { GLFW_KEY_F10, KeyboardKey::KEY_F10 },
    { GLFW_KEY_F11, KeyboardKey::KEY_F11 },
    { GLFW_KEY_F12, KeyboardKey::KEY_F12 },
    { GLFW_KEY_PAGE_UP, KeyboardKey::KEY_PAGE_UP },
    { GLFW_KEY_PAGE_DOWN, KeyboardKey::KEY_PAGE_DOWN },
    { GLFW_KEY_END, KeyboardKey::KEY_END_FILE },
    { GLFW_KEY_LEFT, KeyboardKey::KEY_ARROW_LEFT },
    { GLFW_KEY_RIGHT, KeyboardKey::KEY_ARROW_RIGHT },
    { GLFW_KEY_UP, KeyboardKey::KEY_ARROW_UP },
    { GLFW_KEY_DOWN, KeyboardKey::KEY_ARROW_DOWN },
    { GLFW_KEY_SPACE, KeyboardKey::KEY_SPACE },
    { GLFW_KEY_ESCAPE, KeyboardKey::KEY_ESCAPE }
};

const std::unordered_map<int, MouseButton> GLFW_TO_MOUSE_BUTTON
    = { { GLFW_MOUSE_BUTTON_LEFT, MouseButton::LEFT },
          { GLFW_MOUSE_BUTTON_RIGHT, MouseButton::RIGHT },
          { GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::MIDDLE } };

static void key_callback(GLFWwindow* win, int key, int, int action, int)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (!wld)
        return;

    auto events_opt = wld->get<EventChannel<InputEvent>>();

    if (!events_opt)
        return;
    auto& events = events_opt->get();

    auto k = GLFW_TO_KEYBOARD_KEYS.find(key);
    auto s = GLFW_TO_REGISTRYSTATE.find(action);

    if (k != GLFW_TO_KEYBOARD_KEYS.end() && s != GLFW_TO_REGISTRYSTATE.end()) {
        events.push({ InputEventType::KEYBOARD, { k->second, s->second } });
    }
}

static void mouse_button_callback(GLFWwindow* win, int button, int action, int)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (!wld)
        return;
    auto events_opt = wld->get<EventChannel<InputEvent>>();

    if (!events_opt)
        return;
    auto& events = events_opt->get();

    auto b = GLFW_TO_MOUSE_BUTTON.find(button);
    auto s = GLFW_TO_REGISTRYSTATE.find(action);
    if (b != GLFW_TO_MOUSE_BUTTON.end() && s != GLFW_TO_REGISTRYSTATE.end()) {
        InputEvent event;
        event.type = InputEventType::MOUSE;
        event.mouse.type = MouseEventType::BUTTON;
        event.mouse.button = { b->second, s->second };
        events.push(event);
    }
}

static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (!wld)
        return;
    auto events_opt = wld->get<EventChannel<InputEvent>>();

    if (!events_opt)
        return;
    auto& events = events_opt->get();
    InputEvent event;
    event.type = InputEventType::MOUSE;
    event.mouse.type = MouseEventType::MOUSEPOS;
    event.mouse.pos = { xpos, ypos };
    events.push(event);
}

static void setup_events_callbacks(World& wld)
{
    GLFWwindow* win = wld.get<GLFWwindow*>()->get();

    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPosCallback(win, cursor_position_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
}

void GLFWInputPlugin::plug(App::Builder& builder) const
{
    builder.add_startup_system(System(setup_events_callbacks));
}
