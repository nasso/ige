#include "ige/graphics/WindowPlugin.hpp"
#include "ige/core/World.hpp"
#include "igepch.hpp"

namespace ige::graphics {

WindowPlugin::WindowPlugin(core::World&)
{
    // TODO
    std::cout << "Window plugin is loading!" << std::endl;
}

WindowPlugin::~WindowPlugin()
{
    std::cout << "Window plugin is unloading!" << std::endl;
}

}
