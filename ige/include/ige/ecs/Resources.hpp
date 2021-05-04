/*
** EPITECH PROJECT, 2021
** ige
** File description:
** Resources
*/

#ifndef A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6
#define A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6

#include <concepts>

namespace ige {
namespace ecs {

    template <typename T>
    concept Resource = std::movable<T>;

}
}

#endif /* A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6 */
