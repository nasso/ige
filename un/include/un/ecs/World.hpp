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

    class World {
    public:
        using EntityId = std::uint64_t;

        EntityId spawn();

        template <typename T, typename... Args,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        T& set(Args&&... args)
        {
            impl::TypeId id = impl::type_id<T>();

            auto any = un::core::Any::from<T>(std::forward<Args>(args)...);
            T& res = any.as<T>();

            m_resources.erase(id);
            m_resources.insert(std::make_pair(id, std::move(any)));
            return res;
        }

        template <typename T,
            typename = std::enable_if_t<std::is_object<T>::value>>
        rtl::Option<T&> get()
        {
            impl::TypeId id = impl::type_id<T>();

            auto it = m_resources.find(id);

            if (it != m_resources.end()) {
                return rtl::some(it->second.as<T>());
            } else {
                return {};
            }
        }

        template <typename T,
            typename... Args,
            typename Storage = un::ecs::MapStorage<EntityId, T>,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        void addComponent(EntityId ent, Args&&... args)
        {
            Storage& strg = get<Storage>()
                                .unwrap_or_else([]() {
                                    return set<Storage>();
                                });

            strg.set(ent, std::forward<Args>(args)...);
        }

        template <typename T,
            typename... Args,
            typename Storage = un::ecs::MapStorage<EntityId, T>,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        rtl::Option<T&> getComponent(EntityId ent)
        {
            return get<Storage>()
                .and_then([=](Storage& strg) {
                    return strg.get(ent);
                });
        }

        template <typename T,
            typename... Args,
            typename Storage = un::ecs::MapStorage<EntityId, T>,
            typename = std::enable_if_t<std::is_object<T>::value
                && std::is_constructible<T, Args...>::value>>
        rtl::Option<const T&> getComponent(EntityId ent) const
        {
            return get<Storage>()
                .and_then([=](const Storage& strg) {
                    return strg.get(ent);
                });
        }

    private:
        EntityId m_last_entity = 0;
        std::unordered_map<impl::TypeId, un::core::Any> m_resources;
    };

}
}

#endif /* B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A */
