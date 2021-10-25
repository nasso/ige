#ifndef C01AD257_208F_403B_BE58_A9DC00BD4D9B
#define C01AD257_208F_403B_BE58_A9DC00BD4D9B

#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/utility/Platform.hpp"
#include "ige/utility/Types.hpp"
#include <concepts>
#include <functional>

namespace ige::ecs {

class World;

/**
 * @brief Concept defining a Plugin.
 *
 * A plugin is any type constructible from a lvalue reference to a World, and
 * satisfying the requirements of the concept for Component.
 */
template <class P>
concept Plugin = Component<P> && std::constructible_from<P, ige::ecs::World&>;

/**
 * @brief Concept representing a term for a query.
 */
template <class T>
concept QueryTerm = Component<T>;

template <class T>
struct Changed {
    u32 tick = 0;
};

template <class T>
struct TermResHelper {
    using type = T;
};

template <class T>
struct TermResHelper<Changed<T>> {
    using type = T;
};

template <class T>
using TermRes = typename TermResHelper<T>::type;

/**
 * @brief A world is a container for entities, components and systems.
 *
 * @see Entity
 * @see Component
 * @see Plugin
 */
class IGE_API World {
public:
    /**
     * @brief Run one step of the simulation.
     *
     * Executes all scheduled systems.
     */
    void update();

    template <Component... Cs>
    Entity spawn(Cs&&...);

    template <Plugin P>
    bool load();

    template <Component C>
    const C* get() const;

    template <Component C>
    C* get_mut();

    template <Component C>
    const C* get(Entity) const;

    template <Component C>
    C* get_mut(Entity);

    template <Component... Cs, std::invocable<const Cs&...> F>
    void for_each(F&& f) const;

    template <QueryTerm... Ts, std::invocable<TermRes<Ts>...> F>
    void system(F&& f);

private:
    u32 m_tick = 0;
};

}

#include "World.inl"

#endif /* C01AD257_208F_403B_BE58_A9DC00BD4D9B */
