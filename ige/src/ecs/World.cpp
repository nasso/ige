#include "ige/ecs/World.hpp"
#include "ige/ecs/Resources.hpp"

namespace ige {
namespace ecs {

    World::World(Resources res)
        : m_resources(std::move(res))
    {
    }

    World::EntityRef::EntityRef(World& wld, EntityId id)
        : m_wld(&wld)
        , m_id(id)
    {
    }

    bool World::EntityRef::operator==(const EntityRef& other) const
    {
        return m_id == other.m_id && m_wld == other.m_wld;
    }

    bool World::EntityRef::operator!=(const EntityRef& other) const
    {
        return !(*this == other);
    }

    bool World::EntityRef::remove()
    {
        return m_wld->remove_entity(m_id);
    }

    EntityId World::EntityRef::id() const
    {
        return m_id;
    }

    bool World::remove_entity(EntityId ent)
    {
        for (auto& comp : m_components) {
            comp.second(ent);
        }

        return true;
    }

}
}
