#ifndef CDA81CD5_ED9F_40DC_A242_5F2A4581DA06
#define CDA81CD5_ED9F_40DC_A242_5F2A4581DA06

#include "ige/ecs/World.hpp"

namespace ige::ecs {

/**
 * @brief A SystemBuilder is a temporary objects used when constructing systems.
 *
 * @see World::system
 */
template <Component... Cs>
class SystemBuilder {
public:
    explicit SystemBuilder(World& world)
        : m_world(world)
    {
    }

    // TODO: union<Cs..., Css...> to remove duplicates

    template <Component... Css>
    SystemBuilder<Cs..., Css...> all();

    template <std::invocable<Cs&...> F>
    void each(F&& f);

private:
    World& m_world;
};

}

#include "System.inl"

#endif /* CDA81CD5_ED9F_40DC_A242_5F2A4581DA06 */
