#include "ige/plugin/WindowPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/InputPlugin.hpp"

#include <GLFW/glfw3.h>
#include <stdexcept>
#include <unordered_set>

#ifdef IGE_OPENGL
#include "glad/gl.h"
#endif

using ige::core::App;
using ige::core::EventChannel;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::input::ControllerAxis;
using ige::plugin::input::ControllerButton;
using ige::plugin::input::ControllerEventType;
using ige::plugin::input::InputEvent;
using ige::plugin::input::InputEventType;
using ige::plugin::input::InputManager;
using ige::plugin::input::InputRegistryState;
using ige::plugin::input::KeyboardKey;
using ige::plugin::input::MouseButton;
using ige::plugin::input::MouseEvent;
using ige::plugin::input::MouseEventType;
using ige::plugin::window::WindowEvent;
using ige::plugin::window::WindowEventKind;
using ige::plugin::window::WindowInfo;
using ige::plugin::window::WindowPlugin;
using ige::plugin::window::WindowSettings;

const std::unordered_map<int, InputRegistryState> GLFW_TO_REGISTRY_STATE = {
    { GLFW_PRESS, InputRegistryState::PRESSED },
    { GLFW_RELEASE, InputRegistryState::RELEASED },
};

const std::unordered_map<int, KeyboardKey> GLFW_TO_KEYBOARD_KEYS = {
    { GLFW_KEY_0, KeyboardKey::KEY_0 },
    { GLFW_KEY_1, KeyboardKey::KEY_1 },
    { GLFW_KEY_2, KeyboardKey::KEY_2 },
    { GLFW_KEY_3, KeyboardKey::KEY_3 },
    { GLFW_KEY_4, KeyboardKey::KEY_4 },
    { GLFW_KEY_5, KeyboardKey::KEY_5 },
    { GLFW_KEY_6, KeyboardKey::KEY_6 },
    { GLFW_KEY_7, KeyboardKey::KEY_7 },
    { GLFW_KEY_8, KeyboardKey::KEY_8 },
    { GLFW_KEY_9, KeyboardKey::KEY_9 },
    { GLFW_KEY_A, KeyboardKey::KEY_A },
    { GLFW_KEY_LEFT_ALT, KeyboardKey::KEY_ALT_LEFT },
    { GLFW_KEY_RIGHT_ALT, KeyboardKey::KEY_ALT_RIGHT },
    { GLFW_KEY_B, KeyboardKey::KEY_B },
    { GLFW_KEY_BACKSPACE, KeyboardKey::KEY_BACK_SPACE },
    { GLFW_KEY_C, KeyboardKey::KEY_C },
    { GLFW_KEY_D, KeyboardKey::KEY_D },
    { GLFW_KEY_DELETE, KeyboardKey::KEY_DELETE },
    { GLFW_KEY_DOWN, KeyboardKey::KEY_ARROW_DOWN },
    { GLFW_KEY_E, KeyboardKey::KEY_E },
    { GLFW_KEY_END, KeyboardKey::KEY_END_FILE },
    { GLFW_KEY_ESCAPE, KeyboardKey::KEY_ESCAPE },
    { GLFW_KEY_F, KeyboardKey::KEY_F },
    { GLFW_KEY_F1, KeyboardKey::KEY_F1 },
    { GLFW_KEY_F10, KeyboardKey::KEY_F10 },
    { GLFW_KEY_F11, KeyboardKey::KEY_F11 },
    { GLFW_KEY_F12, KeyboardKey::KEY_F12 },
    { GLFW_KEY_F2, KeyboardKey::KEY_F2 },
    { GLFW_KEY_F3, KeyboardKey::KEY_F3 },
    { GLFW_KEY_F4, KeyboardKey::KEY_F4 },
    { GLFW_KEY_F5, KeyboardKey::KEY_F5 },
    { GLFW_KEY_F6, KeyboardKey::KEY_F6 },
    { GLFW_KEY_F7, KeyboardKey::KEY_F7 },
    { GLFW_KEY_F8, KeyboardKey::KEY_F8 },
    { GLFW_KEY_F9, KeyboardKey::KEY_F9 },
    { GLFW_KEY_G, KeyboardKey::KEY_G },
    { GLFW_KEY_H, KeyboardKey::KEY_H },
    { GLFW_KEY_I, KeyboardKey::KEY_I },
    { GLFW_KEY_INSERT, KeyboardKey::KEY_INSERT },
    { GLFW_KEY_J, KeyboardKey::KEY_J },
    { GLFW_KEY_K, KeyboardKey::KEY_K },
    { GLFW_KEY_L, KeyboardKey::KEY_L },
    { GLFW_KEY_LEFT_CONTROL, KeyboardKey::KEY_CTRL_LEFT },
    { GLFW_KEY_LEFT_SHIFT, KeyboardKey::KEY_SHIFT_LEFT },
    { GLFW_KEY_LEFT, KeyboardKey::KEY_ARROW_LEFT },
    { GLFW_KEY_M, KeyboardKey::KEY_M },
    { GLFW_KEY_N, KeyboardKey::KEY_N },
    { GLFW_KEY_O, KeyboardKey::KEY_O },
    { GLFW_KEY_P, KeyboardKey::KEY_P },
    { GLFW_KEY_PAGE_DOWN, KeyboardKey::KEY_PAGE_DOWN },
    { GLFW_KEY_PAGE_UP, KeyboardKey::KEY_PAGE_UP },
    { GLFW_KEY_Q, KeyboardKey::KEY_Q },
    { GLFW_KEY_R, KeyboardKey::KEY_R },
    { GLFW_KEY_RIGHT_CONTROL, KeyboardKey::KEY_CTRL_RIGHT },
    { GLFW_KEY_RIGHT_SHIFT, KeyboardKey::KEY_SHIFT_RIGHT },
    { GLFW_KEY_RIGHT, KeyboardKey::KEY_ARROW_RIGHT },
    { GLFW_KEY_S, KeyboardKey::KEY_S },
    { GLFW_KEY_SPACE, KeyboardKey::KEY_SPACE },
    { GLFW_KEY_T, KeyboardKey::KEY_T },
    { GLFW_KEY_TAB, KeyboardKey::KEY_TAB },
    { GLFW_KEY_U, KeyboardKey::KEY_U },
    { GLFW_KEY_UP, KeyboardKey::KEY_ARROW_UP },
    { GLFW_KEY_V, KeyboardKey::KEY_V },
    { GLFW_KEY_W, KeyboardKey::KEY_W },
    { GLFW_KEY_X, KeyboardKey::KEY_X },
    { GLFW_KEY_Y, KeyboardKey::KEY_Y },
    { GLFW_KEY_Z, KeyboardKey::KEY_Z },
};

const std::unordered_map<int, ControllerButton> GLFW_TO_CONTROLLER_BUTTONS = {
    { GLFW_GAMEPAD_BUTTON_A, ControllerButton::A },
    { GLFW_GAMEPAD_BUTTON_B, ControllerButton::B },
    { GLFW_GAMEPAD_BUTTON_X, ControllerButton::X },
    { GLFW_GAMEPAD_BUTTON_Y, ControllerButton::Y },
    { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER, ControllerButton::LEFT_BUMPER },
    { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ControllerButton::RIGHT_BUMPER },
    { GLFW_GAMEPAD_BUTTON_BACK, ControllerButton::BACK },
    { GLFW_GAMEPAD_BUTTON_START, ControllerButton::START },
    { GLFW_GAMEPAD_BUTTON_GUIDE, ControllerButton::GUIDE },
    { GLFW_GAMEPAD_BUTTON_LEFT_THUMB, ControllerButton::LEFT_THUMB },
    { GLFW_GAMEPAD_BUTTON_RIGHT_THUMB, ControllerButton::RIGHT_THUMB },
    { GLFW_GAMEPAD_BUTTON_DPAD_UP, ControllerButton::DPAD_UP },
    { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT, ControllerButton::DPAD_RIGHT },
    { GLFW_GAMEPAD_BUTTON_DPAD_DOWN, ControllerButton::DPAD_DOWN },
    { GLFW_GAMEPAD_BUTTON_DPAD_LEFT, ControllerButton::DPAD_RIGHT },
};

const std::unordered_map<int, ControllerAxis> GLFW_TO_CONTROLLER_AXES = {
    { GLFW_GAMEPAD_AXIS_LEFT_X, ControllerAxis::LEFT_X },
    { GLFW_GAMEPAD_AXIS_LEFT_Y, ControllerAxis::LEFT_Y },
    { GLFW_GAMEPAD_AXIS_RIGHT_X, ControllerAxis::RIGHT_X },
    { GLFW_GAMEPAD_AXIS_RIGHT_Y, ControllerAxis::RIGHT_Y },
    { GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, ControllerAxis::LEFT_TRIGGER },
    { GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, ControllerAxis::RIGHT_TRIGGER },
};

const std::unordered_map<int, MouseButton> GLFW_TO_MOUSE_BUTTON = {
    { GLFW_MOUSE_BUTTON_LEFT, MouseButton::LEFT },
    { GLFW_MOUSE_BUTTON_RIGHT, MouseButton::RIGHT },
    { GLFW_MOUSE_BUTTON_MIDDLE, MouseButton::MIDDLE },
};

static void key_callback(GLFWwindow* win, int key, int, int action, int)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (auto events = wld->get<EventChannel<InputEvent>>()) {
        auto k = GLFW_TO_KEYBOARD_KEYS.find(key);
        auto s = GLFW_TO_REGISTRY_STATE.find(action);

        if (k != GLFW_TO_KEYBOARD_KEYS.end()
            && s != GLFW_TO_REGISTRY_STATE.end()) {
            events->push({
                InputEventType::KEYBOARD,
                { k->second, s->second },
            });
        }
    }
}

static void mouse_button_callback(GLFWwindow* win, int button, int action, int)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (auto events = wld->get<EventChannel<InputEvent>>()) {
        auto b = GLFW_TO_MOUSE_BUTTON.find(button);
        auto s = GLFW_TO_REGISTRY_STATE.find(action);

        if (b != GLFW_TO_MOUSE_BUTTON.end()
            && s != GLFW_TO_REGISTRY_STATE.end()) {
            InputEvent event;
            event.type = InputEventType::MOUSE;
            event.mouse.type = MouseEventType::BUTTON;
            event.mouse.button = { b->second, s->second };
            events->push(event);
        }
    }
}

static void scroll_callback(GLFWwindow* win, double dx, double dy)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (auto events = wld->get<EventChannel<InputEvent>>()) {
        InputEvent event;
        event.type = InputEventType::MOUSE;
        event.mouse.type = MouseEventType::SCROLL;
        event.mouse.scroll = {
            static_cast<float>(dx),
            static_cast<float>(dy),
        };
        events->push(event);
    }
}

static void cursor_position_callback(GLFWwindow* win, double xpos, double ypos)
{
    World* wld = static_cast<World*>(glfwGetWindowUserPointer(win));

    if (auto events = wld->get<EventChannel<InputEvent>>()) {
        InputEvent event;
        event.type = InputEventType::MOUSE;
        event.mouse.type = MouseEventType::MOUSE_MOVE;
        event.mouse.pos = {
            static_cast<float>(xpos),
            static_cast<float>(ypos),
        };
        events->push(event);
    }
}

static void init_glfw_system(World&)
{
    if (!glfwInit()) {
        throw std::runtime_error("Unable to initialize GLFW");
    }
}

static void window_resize_callback(GLFWwindow* win, int width, int height)
{
    World& wld = *static_cast<World*>(glfwGetWindowUserPointer(win));

    auto& info = wld.get_or_emplace<WindowInfo>();
    info.width = width;
    info.height = height;

#ifdef IGE_OPENGL
    glViewport(0, 0, width, height);
#endif
}

static void create_window_system(World& wld)
{
    auto settings = wld.get<WindowSettings>();

    if (!settings) {
        return;
    }

#ifdef IGE_OPENGL
    // Specify which version of OpenGL we want: 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // Create the GLFW window
    GLFWwindow* win = glfwCreateWindow(
        settings->width, settings->height, settings->title.c_str(), NULL, NULL);
    if (!win) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

    glfwSetWindowUserPointer(win, &wld);
    glfwSetWindowSizeCallback(win, window_resize_callback);

#ifdef IGE_OPENGL
    // Make the window's OpenGL context current
    glfwMakeContextCurrent(win);
    gladLoadGL(glfwGetProcAddress);
    glViewport(0, 0, settings->width, settings->height);
#endif

    // Enable V-Sync
    glfwSwapInterval(1);

    wld.insert(win);
    wld.insert(WindowInfo { settings->width, settings->height });

    glfwSetWindowUserPointer(win, &wld);
    glfwSetKeyCallback(win, key_callback);
    glfwSetCursorPosCallback(win, cursor_position_callback);
    glfwSetMouseButtonCallback(win, mouse_button_callback);
    glfwSetScrollCallback(win, scroll_callback);
}

static void destroy_window_system(World& wld)
{
    auto win_opt = wld.remove<GLFWwindow*>();

    if (!win_opt) {
        return;
    }

    GLFWwindow* win = *win_opt;
    glfwDestroyWindow(win);
}

static void terminate_glfw_system(World&)
{
    glfwTerminate();
}

static void update_window_system(World& wld)
{
    if (auto win = wld.get<GLFWwindow*>()) {
        if (glfwWindowShouldClose(*win)) {
            if (auto channel = wld.get<EventChannel<WindowEvent>>()) {
                channel->push(WindowEvent {
                    WindowEventKind::WindowClose,
                });
            }
        }

        glfwSwapBuffers(*win);
    }
}

static void poll_events_system(World&)
{
    glfwPollEvents();
}

static void update_gamepads(World& wld)
{
    auto manager = wld.get<InputManager<>>();

    if (!manager) {
        return;
    }

    for (int jid = GLFW_JOYSTICK_1; jid <= GLFW_JOYSTICK_LAST; jid++) {
        if (glfwJoystickIsGamepad(jid)) {
            auto controller = manager->controller(jid);

            if (!controller) {
                controller = &manager->add_controller(jid);
                return;
            }

            GLFWgamepadstate state;

            if (glfwGetGamepadState(jid, &state)) {
                for (int button = 0; button < GLFW_GAMEPAD_BUTTON_LAST;
                     button++) {
                    auto b = GLFW_TO_CONTROLLER_BUTTONS.find(button);
                    auto s = GLFW_TO_REGISTRY_STATE.find(state.buttons[button]);

                    if (b != GLFW_TO_CONTROLLER_BUTTONS.end()
                        && s != GLFW_TO_REGISTRY_STATE.end()) {
                        controller->set_state(b->second, s->second);
                    }
                }

                for (int axis = 0; axis < GLFW_GAMEPAD_AXIS_LAST; axis++) {
                    auto a = GLFW_TO_CONTROLLER_AXES.find(axis);

                    if (a != GLFW_TO_CONTROLLER_AXES.end()) {
                        controller->set_axis_value(a->second, state.axes[axis]);
                    }
                }
            }
        }
    }
}

void WindowPlugin::plug(App::Builder& builder) const
{
    builder.emplace<EventChannel<WindowEvent>>();
    builder.add_startup_system(System::from(init_glfw_system));
    builder.add_startup_system(System::from(create_window_system));
    builder.add_cleanup_system(System::from(destroy_window_system));
    builder.add_cleanup_system(System::from(terminate_glfw_system));
    builder.add_system(System::from(update_window_system));
    builder.add_system(System::from(poll_events_system));
    builder.add_system(System::from(update_gamepads));
}
