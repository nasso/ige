#include <ige.hpp>

using ige::ecs::World;
using ige::graphics::Window;
using ige::graphics::WindowPlugin;

int main()
{
    World world;

    world.load<WindowPlugin>();

    world.spawn(Window("Example Window", 640, 480));
}
