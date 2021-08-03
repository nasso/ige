#ifndef A8E6AC32_5F53_4DA0_89E8_D7F4492032B0
#define A8E6AC32_5F53_4DA0_89E8_D7F4492032B0

#include "ige/core/Types.hpp"

namespace ige::ecs {

using EntityId = u64;

class Entity {
public:
    inline Entity(EntityId id = 0);

    inline operator EntityId() const;

private:
    EntityId m_id = 0;
};

}

#include "Entity.inl"

#endif /* A8E6AC32_5F53_4DA0_89E8_D7F4492032B0 */
