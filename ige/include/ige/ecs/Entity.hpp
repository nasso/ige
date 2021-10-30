#ifndef A8E6AC32_5F53_4DA0_89E8_D7F4492032B0
#define A8E6AC32_5F53_4DA0_89E8_D7F4492032B0

#include "ige/utility/Types.hpp"

namespace ige::ecs {

class Entity {
public:
    inline Entity(u32 id = 0, u16 gen = 0)
        : m_id((static_cast<u64>(id) << 32) | (static_cast<u64>(gen) << 16)) {};

    inline u32 id() const { return m_id >> 32; }

    inline u16 gen() const { return (m_id >> 16) & 0xFFFF; }

private:
    u64 m_id = 0;
};

}

#endif /* A8E6AC32_5F53_4DA0_89E8_D7F4492032B0 */
