#ifndef C198ABD2_597E_4EDC_A112_EE5F956C57EE
#define C198ABD2_597E_4EDC_A112_EE5F956C57EE

#include <concepts>

namespace ige {
namespace math {

    template <std::regular T>
    struct Vector3 {
        T x;
        T y;
        T z;

        explicit constexpr Vector3(T all) noexcept
            : x(all)
            , y(all)
            , z(all)
        {
        }

        constexpr Vector3(T x, T y, T z) noexcept
            : x(x)
            , y(y)
            , z(z)
        {
        }

        constexpr Vector3(const Vector3&) = default;
        constexpr Vector3& operator=(const Vector3&) = default;

        constexpr bool operator==(const Vector3&) const = default;
    };

    using Vec3 = Vector3<float>;

}
}

#endif /* C198ABD2_597E_4EDC_A112_EE5F956C57EE */
