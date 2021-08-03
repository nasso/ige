#include "Entity.hpp"

namespace ige::ecs {

Entity::Entity(EntityId id)
    : m_id(id)
{
}

Entity::operator EntityId() const
{
    return m_id;
}

}
