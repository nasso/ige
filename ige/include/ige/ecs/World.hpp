#ifndef B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A
#define B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A

#include "ige/core/Any.hpp"
#include "ige/core/TypeId.hpp"
#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/MapStorage.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/VecStorage.hpp"
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

namespace ige::ecs {

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
        C* get_component()
        {
            return m_wld->get_component<C>(m_id);
        }

        template <Component C>
        const C* get_component() const
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
        decltype(auto) get_component_bundle()
        {
            return m_wld->get_component_bundle<Cs...>();
        }

        template <Component... Cs>
        decltype(auto) get_component_bundle() const
        {
            return m_wld->get_component_bundle<Cs...>();
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

    bool remove_entity(const EntityId&);

    template <Component... Cs>
    EntityRef create_entity(Cs&&... components)
    {
        EntityRef entity(*this, m_entities.allocate());

        m_generation.set(entity.id().index(), entity.id().generation());

        entity.add_all_components(std::forward<Cs>(components)...);
        return entity;
    }

    bool exists(const EntityId&);

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
    R* get()
    {
        return m_resources.get<R>();
    }

    template <Resource R>
    const R* get() const
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
        return get<StorageOf<C>>();
    }

    template <Component C>
    decltype(auto) get_component_storage() const
    {
        return get<StorageOf<C>>();
    }

    template <Component C>
    C& add_component(const EntityId& ent, C comp)
    {
        return emplace_component<C>(ent, std::move(comp));
    }

    template <Component C, typename... Args>
    requires std::constructible_from<C, Args...> C&
    emplace_component(const EntityId& ent, Args&&... args)
    {
        auto& strg = get_or_emplace<StorageOf<C>>();

        strg.set(ent.index(), std::forward<Args>(args)...);

        if (m_components.find(core::type_id<C>()) == m_components.end()) {
            m_components.emplace(core::type_id<C>(), [&](EntityId ent) {
                return remove_component<C>(ent).has_value();
            });
        }

        return *strg.get(ent.index());
    }

    template <Component C>
    C* get_component(const EntityId& ent)
    {
        if (auto strg = get<StorageOf<C>>()) {
            return strg->get(ent.index());
        } else {
            return nullptr;
        }
    }

    template <Component C>
    const C* get_component(const EntityId& ent) const
    {
        if (auto strg = get<StorageOf<C>>()) {
            return strg->get(ent.index());
        } else {
            return nullptr;
        }
    }

    template <Component C>
    std::optional<C> remove_component(const EntityId& ent)
    {
        if (auto strg = get<StorageOf<C>>()) {
            return strg->remove(ent.index());
        } else {
            return std::nullopt;
        }
    }

    template <Component... Cs>
    std::optional<std::tuple<Cs&...>>
    get_component_bundle(const EntityId& entity)
    {
        auto components = std::make_tuple(get_component<Cs>(entity)...);

        bool has_all = ((std::get<Cs*>(components) != nullptr) && ...);

        std::optional<std::tuple<Cs&...>> bundle;

        if (has_all) {
            bundle.emplace(*std::get<Cs*>(components)...);
        }

        return bundle;
    }

    template <Component... Cs>
    std::optional<std::tuple<const Cs&...>>
    get_component_bundle(const EntityId& entity) const
    {
        auto components = std::make_tuple(get_component<Cs>(entity)...);

        bool has_all = ((std::get<const Cs*>(components) != nullptr) && ...);

        std::optional<std::tuple<const Cs&...>> bundle;

        if (has_all) {
            bundle.emplace(*std::get<const Cs*>(components)...);
        }

        return bundle;
    }

    template <Component C, typename... Args>
    requires std::constructible_from<C, Args...> C&
    get_or_emplace_component(const EntityId& ent, Args&&... args)
    {
        if (auto comp = get_component<C>(ent)) {
            return *comp;
        } else {
            return emplace_component<C>(ent, std::forward<Args>(args)...);
        }
    }

private:
    template <Component... Cs>
    class Query {
    private:
        std::optional<std::tuple<StorageOf<Cs>&...>> m_storages;
        World& m_world;

    public:
        Query(World& world)
            : m_world(world)
        {
            auto strgs = std::make_tuple(world.get_component_storage<Cs>()...);
            bool empty = !((std::get<StorageOf<Cs>*>(strgs) != nullptr) && ...);

            if (!empty) {
                m_storages.emplace(
                    std::tie(*std::get<StorageOf<Cs>*>(strgs)...));
            }
        }

        template <Component C>
        decltype(auto) view_from()
        {
            std::vector<std::tuple<EntityId, Cs&...>> entities;

            if (!m_storages) {
                return entities;
            }

            auto& storage = std::get<StorageOf<C>&>(*m_storages);

            for (const auto& [id, comp] : storage) {
                EntityId ent { id, *m_world.m_generation.get(id) };
                auto components = m_world.get_component_bundle<Cs...>(ent);

                if (components) {
                    entities.emplace_back(ent, std::get<Cs&>(*components)...);
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

    EntityPool m_entities;
    VecStorage<std::uint64_t> m_generation;
    std::unordered_map<core::TypeId, CompRemover> m_components;
    Resources m_resources;
};

}

#endif /* B1C7D01B_EFEC_457F_B2AD_A4234DA7F87A */
