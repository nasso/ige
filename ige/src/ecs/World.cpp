#include "ige/ecs/World.hpp"
#include "ige/ecs/System.hpp"
#include "ige/utility/Control.hpp"
#include <cstdlib>

namespace ige::ecs {

Entity World::empty() { return IGE_TODO(Entity); }

void World::attach(Entity, u64) { IGE_TODO(); }

void World::detach(Entity, u64) { IGE_TODO(); }

Query<> World::query() { return Query(*this); }

SystemBuilder<> World::system() { return SystemBuilder(*this); }

void World::update()
{
    m_tick++;
    IGE_TODO();
}

}
