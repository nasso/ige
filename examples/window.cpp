#include <ige.hpp>
#include <iostream>
#include <utility>

using ige::ecs::World;
using ige::graphics::Window;
using ige::graphics::WindowPlugin;

int main()
{
    // The world is the main container for all entities and systems that make up
    // the game.
    World world;

    // Plugins are used to extend the functionality of the engine.
    // In this example we load the window plugin that will be used to create a
    // window and render the game.
    world.load<WindowPlugin>();

    // The window plugin provides the Window component.
    // "spawn" is used to create a new entity.
    world.spawn(Window("Example Window", 640, 480));

    bool running = true;

    // "system" creates a system to be run on entities matching a query.
    // Here, we create a system to detect if the window has been closed.
    // If the window is closed, the program will exit.
    world //
        .system()
        .all<Window>()
        .each([&](const Window& window) {
            if (window.closed) {
                running = false;
            }
        });

    // This is the main game loop.
    while (running) {
        world.update();
    }
}
