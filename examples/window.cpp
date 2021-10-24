#include <ige.hpp>
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

    // This is the main game loop.
    for (bool running = true; running;) {
        // "for_each" is used to iterate over all entities with the Window
        // component.
        world.for_each<Window>([&](const Window& window) {
            // break if the window was closed
            running &= !window.closed;
        });
    }
}
