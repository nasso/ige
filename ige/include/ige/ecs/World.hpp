#ifndef B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A
#define B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A

#include "ige/core/Any.hpp"
#include "ige/ecs/Component.hpp"
#include "ige/ecs/MapStorage.hpp"
#include "ige/ecs/Resources.hpp"
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <unordered_set>

namespace ige {
namespace ecs {

    using EntityId = std::uint64_t;

    template <Component C>
    struct ComponentStorage {
        using Type = MapStorage<EntityId, C>;
    };

    class World {
    public:
        class EntityRef {
        public:
            EntityRef(World&, EntityId);
            EntityRef(const EntityRef&) = default;
            EntityRef& operator=(const EntityRef&) = default;

            bool operator==(const EntityRef&) const;
            bool operator!=(const EntityRef&) const;
            bool remove();

            template <Component T, typename... Args>
            requires std::constructible_from<T, Args...> T& add_component(
                Args&&... args)
            {
                return m_wld->add_component<T>(
                    m_id, std::forward<Args>(args)...);
            }

            template <Component T>
            std::optional<std::reference_wrapper<T>> get_component()
            {
                return m_wld->get_component<T>(m_id);
            }

            template <Component T>
            std::optional<std::reference_wrapper<const T>> get_component() const
            {
                return m_wld->get_component<T>(m_id);
            }

            template <Component T>
            std::optional<T> remove_component()
            {
                return m_wld->remove_component<T>(m_id);
            }

            EntityId id()
            {
                return m_id;
            }

        private:
            World* m_wld;
            EntityId m_id;
        };

        World(Resources = {});

        EntityRef create_entity();
        bool remove_entity(EntityId);

        template <Resource T, typename... Args>
        requires std::constructible_from<T, Args...> T& set(Args&&... args)
        {
            return m_resources.set<T>(std::forward<Args>(args)...);
        }

        template <Resource T>
        std::optional<std::reference_wrapper<T>> get()
        {
            return m_resources.get<T>();
        }

        template <Resource T>
        std::optional<std::reference_wrapper<const T>> get() const
        {
            return m_resources.get<T>();
        }

        template <Resource T>
        std::optional<T> remove()
        {
            return m_resources.remove<T>();
        }

        template <Component T, typename... Args>
        requires std::constructible_from<T, Args...> T& add_component(
            EntityId ent, Args&&... args)
        {
            using Storage = typename ComponentStorage<T>::Type;

            auto optstrg = get<Storage>();

            if (!get<Storage>().has_value()) {
                set<Storage>();
            }

            auto& strg = get<Storage>()->get();

            strg.set(std::move(ent), std::forward<Args>(args)...);

            if (m_components.find(impl::type_id<T>()) == m_components.end()) {
                m_components.emplace(impl::type_id<T>(), [&](EntityId ent) {
                    return remove_component<T>(ent).has_value();
                });
            }

            return strg.get(ent)->get();
        }

        template <Component T>
        std::optional<std::reference_wrapper<T>> get_component(EntityId ent)
        {
            using Storage = typename ComponentStorage<T>::Type;

            if (auto strg = get<Storage>()) {
                return strg->get().get(ent);
            } else {
                return {};
            }
        }

        template <Component T>
        std::optional<std::reference_wrapper<const T>> get_component(
            EntityId ent) const
        {
            using Storage = typename ComponentStorage<T>::Type;

            if (auto strg = get<Storage>()) {
                return strg->get().get(ent);
            } else {
                return {};
            }
        }

        template <Component T>
        std::optional<T> remove_component(EntityId ent)
        {
            using Storage = typename ComponentStorage<T>::Type;

            if (auto strg = get<Storage>()) {
                return strg->get().remove(ent);
            } else {
                return {};
            }
        }

    private:
        using CompRemover = std::function<bool(EntityId)>;

        EntityId m_last_entity = 0;
        std::unordered_map<impl::TypeId, CompRemover> m_components;
        Resources m_resources;
    };

}
}

#endif /* B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A */
