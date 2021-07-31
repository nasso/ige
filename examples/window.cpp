#include <ige.hpp>

using ige::core::World;
using ige::graphics::Window;
using ige::graphics::WindowPlugin;

int main()
{
    World world;

    world.load<WindowPlugin>();

    world.spawn(Window("Example Window", 640, 480));
}
