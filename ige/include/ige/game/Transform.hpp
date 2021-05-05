/*
** EPITECH PROJECT, 2021
** ige
** File description:
** Transform
*/

#ifndef F4DF8A5F_1CCD_443F_8D71_8A439340E94F
#define F4DF8A5F_1CCD_443F_8D71_8A439340E94F

#include "ige/math/Vector.hpp"

namespace ige {
namespace game {

    struct Transform {
        math::Vec3 translation = math::Vec3(0.0f);
        math::Vec3 rotation = math::Vec3(0.0f);
        math::Vec3 scale = math::Vec3(1.0f);

        static constexpr Transform look_at(math::Vec3 position,
            math::Vec3 target, math::Vec3 up = math::Vec3(0.0f, 1.0f, 0.0f))
        {
            Transform xform;

            xform.translation = position;
            // todo
            return xform;
        }

        constexpr explicit Transform() = default;
    };

}
}

#endif /* F4DF8A5F_1CCD_443F_8D71_8A439340E94F */
