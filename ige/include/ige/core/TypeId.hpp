#ifndef B7A75855_66BA_42E2_970F_9BDC8EA798EA
#define B7A75855_66BA_42E2_970F_9BDC8EA798EA

#include <cstddef>

namespace ige::core {

namespace detail {

    template <typename T>
    struct type_id_ptr {
        static const T* const id;
    };

    template <typename T>
    const T* const type_id_ptr<T>::id = nullptr;

}

using TypeId = std::size_t;

template <typename T>
constexpr TypeId type_id() noexcept
{
    return reinterpret_cast<TypeId>(&detail::type_id_ptr<T>::id);
}

}

#endif /* B7A75855_66BA_42E2_970F_9BDC8EA798EA */
