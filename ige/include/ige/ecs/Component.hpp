#ifndef D7898513_38A6_437F_A2C8_820672FA3689
#define D7898513_38A6_437F_A2C8_820672FA3689

#include <concepts>
#include <type_traits>

namespace ige::ecs {

/**
 * @brief Concept satisfied by types representing a "Tag".
 *
 * Tags are simply components that have no data associated with them. They are
 * used to efficiently identify entities.
 */
template <class T>
concept Tag = std::is_empty_v<T>;

/**
 * @brief Concept for Component data types.
 */
template <class C>
concept Component = Tag<C> || std::movable<C>;

}

#endif /* D7898513_38A6_437F_A2C8_820672FA3689 */
