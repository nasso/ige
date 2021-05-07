#ifndef A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6
#define A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6

#include "ige/core/Any.hpp"
#include <concepts>
#include <functional>
#include <optional>
#include <utility>

namespace ige {
namespace ecs {

    namespace impl {
        using TypeId = std::size_t;

        template <typename T>
        struct type_id_ptr {
            static const T* const id;
        };

        template <typename T>
        const T* const type_id_ptr<T>::id = nullptr;

        template <typename T>
        constexpr TypeId type_id() noexcept
        {
            return reinterpret_cast<TypeId>(&type_id_ptr<T>::id);
        }
    }

    template <typename T>
    concept Resource = std::movable<T>;

    class Resources {
    public:
        template <Resource R>
        R& insert(R res)
        {
            impl::TypeId id = impl::type_id<R>();

            return set_any(id, core::Any::from<R>(std::move(res)))
                .template as<R>();
        }

        template <Resource T, typename... Args>
        requires std::constructible_from<T, Args...> T& emplace(Args&&... args)
        {
            impl::TypeId id = impl::type_id<T>();

            return set_any(id, core::Any::from<T>(std::forward<Args>(args)...))
                .template as<T>();
        }

        template <Resource T>
        std::optional<std::reference_wrapper<T>> get()
        {
            impl::TypeId id = impl::type_id<T>();

            if (auto any = get_any(id)) {
                return { any->get().template as<T>() };
            } else {
                return {};
            }
        }

        template <Resource T>
        std::optional<std::reference_wrapper<const T>> get() const
        {
            impl::TypeId id = impl::type_id<T>();

            if (auto any = get_any(id)) {
                return { any->get().template as<T>() };
            } else {
                return {};
            }
        }

        template <Resource T>
        std::optional<T> remove()
        {
            impl::TypeId id = impl::type_id<T>();

            if (auto any = remove_any(id)) {
                return { std::move(any->template as<T>()) };
            } else {
                return {};
            }
        }

    private:
        std::unordered_map<impl::TypeId, core::Any> m_resources;

        core::Any& set_any(impl::TypeId id, core::Any any)
        {
            m_resources.erase(id);
            m_resources.insert(std::make_pair(id, std::move(any)));
            return m_resources.at(id);
        }

        std::optional<std::reference_wrapper<core::Any>> get_any(
            impl::TypeId id)
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                return { it->second };
            } else {
                return {};
            }
        }

        std::optional<std::reference_wrapper<const core::Any>> get_any(
            impl::TypeId id) const
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                return { it->second };
            } else {
                return {};
            }
        }

        std::optional<core::Any> remove_any(impl::TypeId id)
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                auto any = std::move(it->second);
                m_resources.erase(id);
                return { std::move(any) };
            } else {
                return {};
            }
        }
    };

}
}

#endif /* A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6 */
