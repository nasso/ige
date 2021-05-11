#include "ige/plugin/WindowingPlugin.hpp"
#include "ige/core/App.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <unordered_set>

#ifdef IGE_OPENGL
#include <glad/gl.h>
#endif

using ige::core::App;
using ige::core::EventChannel;
using ige::ecs::System;
using ige::ecs::World;
using ige::plugin::WindowEvent;
using ige::plugin::WindowEventKind;
using ige::plugin::WindowInfo;
using ige::plugin::WindowingPlugin;
using ige::plugin::WindowSettings;

static void init_glfw_system(World&)
{
    if (!glfwInit()) {
        throw std::runtime_error("Unable to initialize GLFW");
    }
}

static void create_window_system(World& wld)
{
    auto settings_ref = wld.get<WindowSettings>();

    if (!settings_ref) {
        return;
    }

    WindowSettings& settings = settings_ref->get();

#ifdef IGE_OPENGL
    // Specify which version of OpenGL we want: 3.3 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

    // Create the GLFW window
    GLFWwindow* win = glfwCreateWindow(
        settings.width, settings.height, settings.title.c_str(), NULL, NULL);
    if (!win) {
        glfwTerminate();
        throw std::runtime_error("Unable to create GLFW window");
    }

#ifdef IGE_OPENGL
    // Make the window's OpenGL context current
    glfwMakeContextCurrent(win);
    gladLoadGL(glfwGetProcAddress);
    glViewport(0, 0, settings.width, settings.height);
#endif

    // Enable V-Sync
    glfwSwapInterval(1);

    wld.insert(win);
    wld.insert(WindowInfo { settings.width, settings.height });
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
    auto win_ref = wld.get<GLFWwindow*>();
    if (!win_ref) {
        return;
    }

    GLFWwindow* win = win_ref->get();
    if (glfwWindowShouldClose(win)) {
        if (auto channel = wld.get<EventChannel<WindowEvent>>()) {
            channel->get().push(WindowEvent {
                WindowEventKind::WindowClose,
            });
        }
    }

    glfwSwapBuffers(win);
}

static void poll_events_system(World&)
{
    glfwPollEvents();
}

void WindowingPlugin::plug(App::Builder& builder) const
{
    builder.emplace<EventChannel<WindowEvent>>();
    builder.add_startup_system(System(init_glfw_system));
    builder.add_startup_system(System(create_window_system));
    builder.add_cleanup_system(System(destroy_window_system));
    builder.add_cleanup_system(System(terminate_glfw_system));
    builder.add_system(System(update_window_system));
    builder.add_system(System(poll_events_system));
}
