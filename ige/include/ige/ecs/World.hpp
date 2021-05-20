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
#include <tuple>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace ige {
namespace ecs {

    using EntityId = std::uint64_t;

    template <Component C>
    struct ComponentStorage {
        using Type = MapStorage<EntityId, C>;
    };

    template <Component C>
    using StorageFor = typename ComponentStorage<C>::Type;

    class World {
    public:
        class EntityRef {
        private:
            World* m_wld;
            EntityId m_id;

        public:
            EntityRef(World&, EntityId);
            EntityRef(const EntityRef&) = default;
            EntityRef& operator=(const EntityRef&) = default;

            bool operator==(const EntityRef&) const;
            bool operator!=(const EntityRef&) const;
            bool remove();

            template <Component C>
            C& add_component(C comp)
            {
                return m_wld->add_component(m_id, std::move(comp));
            }

            template <Component C, typename... Args>
            requires std::constructible_from<C, Args...> C&
            emplace_component(Args&&... args)
            {
                return m_wld->emplace_component<C>(
                    m_id, std::forward<Args>(args)...);
            }

            template <Component C>
            std::optional<std::reference_wrapper<C>> get_component()
            {
                return m_wld->get_component<C>(m_id);
            }

            template <Component C>
            std::optional<std::reference_wrapper<const C>> get_component() const
            {
                return m_wld->get_component<C>(m_id);
            }

            template <Component C, typename... Args>
            requires std::constructible_from<C, Args...> C&
            get_or_emplace_component(Args&&... args)
            {
                return m_wld->get_or_emplace_component<C>(
                    m_id, std::forward<Args>(args)...);
            }

            template <Component... Cs>
            std::optional<std::tuple<Cs&...>> get_component_bundle()
            {
                auto components = std::make_tuple(get_component<Cs>()...);

                bool has_all
                    = (std::get<std::optional<std::reference_wrapper<Cs>>>(
                           components)
                           .has_value()
                       && ...);

                std::optional<std::tuple<Cs&...>> bundle;

                if (has_all) {
                    bundle.emplace(
                        std::get<std::optional<std::reference_wrapper<Cs>>>(
                            components)
                            ->get()...);
                }

                return bundle;
            }

            template <Component... Cs>
            std::optional<std::tuple<const Cs&...>> get_component_bundle() const
            {
                auto components = std::make_tuple(get_component<Cs>()...);

                bool has_all
                    = (std::get<
                           std::optional<std::reference_wrapper<const Cs>>>(
                           components)
                           .has_value()
                       && ...);

                std::optional<std::tuple<const Cs&...>> bundle;

                if (has_all) {
                    bundle.emplace(
                        std::get<
                            std::optional<std::reference_wrapper<const Cs>>>(
                            components)
                            ->get()...);
                }

                return bundle;
            }

            template <Component C>
            std::optional<C> remove_component()
            {
                return m_wld->remove_component<C>(m_id);
            }

            void add_all_components()
            {
            }

            template <Component C, Component... Cs>
            void add_all_components(C&& component, Cs&&... rest)
            {
                add_component(std::move(component));
                add_all_components(std::forward<Cs>(rest)...);
            }

            EntityId id() const;
        };

        World(Resources = {});

        bool remove_entity(EntityId);

        template <Component... Cs>
        EntityRef create_entity(Cs&&... components)
        {
            EntityRef entity(*this, ++m_last_entity);

            entity.add_all_components(std::forward<Cs>(components)...);
            return entity;
        }

        template <Resource R>
        R& insert(R res)
        {
            return m_resources.insert(std::move(res));
        }

        template <Resource R, typename... Args>
        requires std::constructible_from<R, Args...> R& emplace(Args&&... args)
        {
            return m_resources.emplace<R>(std::forward<Args>(args)...);
        }

        template <Resource R, typename... Args>
        requires std::constructible_from<R, Args...> R&
        get_or_emplace(Args&&... args)
        {
            return m_resources.get_or_emplace<R>(std::forward<Args>(args)...);
        }

        template <Resource R>
        std::optional<std::reference_wrapper<R>> get()
        {
            return m_resources.get<R>();
        }

        template <Resource R>
        std::optional<std::reference_wrapper<const R>> get() const
        {
            return m_resources.get<R>();
        }

        template <Resource R>
        std::optional<R> remove()
        {
            return m_resources.remove<R>();
        }

        template <Component C>
        decltype(auto) get_component_storage()
        {
            return get<StorageFor<C>>();
        }

        template <Component C>
        decltype(auto) get_component_storage() const
        {
            return get<StorageFor<C>>();
        }

        template <Component C>
        C& add_component(EntityId ent, C comp)
        {
            return emplace_component<C>(ent, std::move(comp));
        }

        template <Component C, typename... Args>
        requires std::constructible_from<C, Args...> C&
        emplace_component(EntityId ent, Args&&... args)
        {
            auto& strg = get_or_emplace<StorageFor<C>>();

            strg.set(std::move(ent), std::forward<Args>(args)...);

            if (m_components.find(impl::type_id<C>()) == m_components.end()) {
                m_components.emplace(impl::type_id<C>(), [&](EntityId ent) {
                    return remove_component<C>(ent).has_value();
                });
            }

            return strg.get(ent)->get();
        }

        template <Component C>
        std::optional<std::reference_wrapper<C>> get_component(EntityId ent)
        {
            if (auto strg = get<StorageFor<C>>()) {
                return strg->get().get(ent);
            } else {
                return std::nullopt;
            }
        }

        template <Component C>
        std::optional<std::reference_wrapper<const C>>
        get_component(EntityId ent) const
        {
            if (auto strg = get<StorageFor<C>>()) {
                return strg->get().get(ent);
            } else {
                return std::nullopt;
            }
        }

        template <Component C, typename... Args>
        requires std::constructible_from<C, Args...> C&
        get_or_emplace_component(EntityId ent, Args&&... args)
        {
            if (auto comp = get_component<C>(ent)) {
                return comp->get();
            } else {
                return emplace_component<C>(ent, std::forward<Args>(args)...);
            }
        }

        template <Component C>
        std::optional<C> remove_component(EntityId ent)
        {
            if (auto strg = get<StorageFor<C>>()) {
                return strg->get().remove(ent);
            } else {
                return {};
            }
        }

    private:
        template <Component... Cs>
        class Query {
        private:
            std::optional<std::tuple<StorageFor<Cs>&...>> m_storages;
            World& m_world;

        public:
            Query(World& world)
                : m_world(world)
            {
                auto opt_strgs
                    = std::make_tuple(world.get_component_storage<Cs>()...);
                bool empty = !(
                    std::get<
                        std::optional<std::reference_wrapper<StorageFor<Cs>>>>(
                        opt_strgs)
                        .has_value()
                    && ...);

                if (!empty) {
                    m_storages.emplace(std::tie(
                        std::get<std::optional<
                            std::reference_wrapper<StorageFor<Cs>>>>(opt_strgs)
                            ->get()...));
                }
            }

            template <Component C>
            decltype(auto) view_from()
            {
                std::vector<std::tuple<EntityRef, Cs&...>> entities;

                if (!m_storages) {
                    return entities;
                }

                auto& storage = std::get<StorageFor<C>&>(*m_storages);

                for (const auto& [id, comp] : storage) {
                    EntityRef entity(m_world, id);

                    auto components = entity.get_component_bundle<Cs...>();

                    if (components) {
                        entities.emplace_back(
                            entity, std::get<Cs&>(*components)...);
                    }
                }

                return entities;
            }
        };

    public:
        template <Component C, Component... Cs>
        decltype(auto) query()
        {
            Query<C, Cs...> q(*this);

            return q.template view_from<C>();
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
