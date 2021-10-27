#include "ige/ecs/World.hpp"

namespace ige::ecs {

void World::update()
{
    m_tick++;
    // TODO
}

SystemBuilder<> World::system() { return SystemBuilder(*this); }

}
