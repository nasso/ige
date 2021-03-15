/*
** EPITECH PROJECT, 2021
** un
** File description:
** World
*/

#ifndef B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A
#define B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A

#include "rtl/Option.hpp"
#include "un/core/Any.hpp"
#include "un/ecs/MapStorage.hpp"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace un {
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

    using EntityId = std::uint64_t;

    template <typename Component>
    struct ComponentStorage {
        using Type = MapStorage<EntityId, Component>;
    };

    class World {
    public:
        EntityId createEntity();
        bool removeEntity(EntityId);

        template <typename T, typename... Args,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        T& set(Args&&... args)
        {
            impl::TypeId id = impl::type_id<T>();

            return setAny(id, core::Any::from<T>(std::forward<Args>(args)...))
                .as<T>();
        }

        template <typename T,
            typename = std::enable_if_t<std::is_object<T>::value>>
        rtl::Option<T&> get()
        {
            impl::TypeId id = impl::type_id<T>();

            return getAny(id)
                .map([](auto& any) -> auto& {
                    return any.as<T>();
                });
        }

        template <typename T,
            typename = std::enable_if_t<std::is_object<T>::value>>
        rtl::Option<const T&> get() const
        {
            impl::TypeId id = impl::type_id<T>();

            return getAny(id)
                .map([](const auto& any) -> const auto& {
                    return any.as<T>();
                });
        }

        template <typename T,
            typename = std::enable_if_t<std::is_object<T>::value>>
        rtl::Option<T> remove()
        {
            return removeAny(impl::type_id<T>())
                .map([](core::Any any) {
                    return any.as<T>();
                });
        }

        template <typename T,
            typename... Args,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        void addComponent(EntityId ent, Args&&... args)
        {
            using Storage = ComponentStorage<T>::Type;

            Storage& strg = get<Storage>()
                                .unwrap_or_else([&]() -> auto& {
                                    return set<Storage>();
                                });

            strg.set(std::move(ent), std::forward<Args>(args)...);

            if (m_components.find(impl::type_id<T>()) == m_components.end()) {
                m_components.emplace(
                    impl::type_id<T>(),
                    [&](EntityId ent) {
                        return removeComponent<T>(ent).is_some();
                    });
            }
        }

        template <typename T, typename... Args,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        rtl::Option<T&> getComponent(EntityId ent)
        {
            using Storage = ComponentStorage<T>::Type;

            return get<Storage>()
                .and_then([=](Storage& strg) {
                    return strg.get(ent);
                });
        }

        template <typename T,
            typename... Args,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        rtl::Option<const T&> getComponent(EntityId ent) const
        {
            using Storage = ComponentStorage<T>::Type;

            return get<Storage>()
                .and_then([=](const Storage& strg) {
                    return strg.get(ent);
                });
        }

        template <typename T,
            typename = std::enable_if_t<std::is_object<T>::value>>
        rtl::Option<T> removeComponent(EntityId ent)
        {
            using Storage = ComponentStorage<T>::Type;

            return get<Storage>()
                .and_then([ent](Storage& strg) {
                    return strg.remove(ent);
                });
        }

    private:
        core::Any& setAny(impl::TypeId id, core::Any any)
        {
            m_resources.erase(id);
            m_resources.insert(std::make_pair(id, std::move(any)));
            return m_resources.at(id);
        }

        rtl::Option<core::Any&> getAny(impl::TypeId id)
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        rtl::Option<const core::Any&> getAny(impl::TypeId id) const
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                return rtl::some(it->second);
            } else {
                return {};
            }
        }

        rtl::Option<core::Any> removeAny(impl::TypeId id)
        {
            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                auto any = std::move(it->second);
                m_resources.erase(id);
                return rtl::some(std::move(any));
            } else {
                return {};
            }
        }

        using CompRemover = std::function<bool(EntityId)>;

        EntityId m_last_entity = 0;
        std::unordered_map<impl::TypeId, core::Any> m_resources;
        std::unordered_map<impl::TypeId, CompRemover> m_components;
    };

}
}

#endif /* B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A */
