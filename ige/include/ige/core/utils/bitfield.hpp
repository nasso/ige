/*
** EPITECH PROJECT, 2021
** ige
** File description:
** bitfield
*/

#ifndef DF43A954_CC27_4B07_ADB1_C50EE78AF09F
#define DF43A954_CC27_4B07_ADB1_C50EE78AF09F

#include <type_traits>

namespace ige {
namespace core {

    namespace utils {

        template <typename E> struct is_bitfield : std::false_type {
        };

        template <typename E>
        constexpr bool is_bitfield_v = is_bitfield<E>::value;

    }

}
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator|(E lhs, E rhs) noexcept
{
    using type = typename std::underlying_type_t<E>;

    return static_cast<E>(static_cast<type>(lhs) | static_cast<type>(rhs));
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator&(E lhs, E rhs) noexcept
{
    using type = typename std::underlying_type_t<E>;

    return static_cast<E>(static_cast<type>(lhs) & static_cast<type>(rhs));
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator^(E lhs, E rhs) noexcept
{
    using type = typename std::underlying_type_t<E>;

    return static_cast<E>(static_cast<type>(lhs) ^ static_cast<type>(rhs));
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator~(E val) noexcept
{
    using type = typename std::underlying_type_t<E>;

    return static_cast<E>(~static_cast<type>(val));
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator|=(E& lhs, E rhs) noexcept
{
    return lhs = lhs | rhs;
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator&=(E& lhs, E rhs) noexcept
{
    return lhs = lhs & rhs;
}

template <typename E,
    typename = std::enable_if_t<ige::core::utils::is_bitfield_v<E>>>
constexpr E operator^=(E& lhs, E rhs) noexcept
{
    return lhs = lhs ^ rhs;
}

#define MAKE_BITFIELD(E)                                                       \
    template <> struct ::ige::core::utils::is_bitfield<E> : ::std::true_type { \
    }

#endif /* DF43A954_CC27_4B07_ADB1_C50EE78AF09F */
