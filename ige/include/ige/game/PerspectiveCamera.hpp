/*
** EPITECH PROJECT, 2021
** ige
** File description:
** PerspectiveCamera
*/

#ifndef D8BF3E52_E300_4A34_B8B5_1446D343CF26
#define D8BF3E52_E300_4A34_B8B5_1446D343CF26

namespace ige {
namespace game {

    struct PerspectiveCamera {
        float fov;
        float near;
        float far;

        constexpr PerspectiveCamera(
            float fov, float near = 0.1f, float far = 1000.0f) noexcept
            : fov(fov)
            , near(near)
            , far(far)
        {
        }
    };

}
}

#endif /* D8BF3E52_E300_4A34_B8B5_1446D343CF26 */
