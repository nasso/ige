#include "World.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include "ige/utility/FamilyGen.hpp"
#include <concepts>
#include <functional>
#include <optional>

namespace ige::ecs {

template <class F>
    requires std::regular_invocable<F, void*> || std::
        regular_invocable<F, void*, bool>
void World::mutate(Entity entity, Entity component, F&& f)
{
    Record* record = get_record_mut(entity);

    IGE_ASSERT(record != nullptr, "\"entity\" is not alive");

    // ensure the entity has the component
    const bool had_component = record->family().has(component.id());

    if (!had_component) {
        add_to_record(*record, component.id());
    }

    // get the column index in which the component is stored
    const auto column = get_component_column(record->family(), component);

    IGE_ASSERT(column.has_value(), "\"component\" is not a component");

    // get or create the table storing the components of the archetype
    Table& table = *touch_table(record->type_mut());

    // ensure the table has enough rows
    if (table.row_count() <= record->row()) {
        table.resize(record->row() + 1);
    }

    // mutate the data
    void* data_ptr = table.cell_mut(*column, record->row());

    if constexpr (std::regular_invocable<F, void*, bool>) {
        f(data_ptr, !had_component);
    } else if constexpr (std::regular_invocable<F, void*>) {
        f(data_ptr);
    }

    // TODO: mark as modified
}

template <Component C>
Entity World::component()
{
    return component(sizeof(C), alignof(C));
}

template <Component C>
Entity World::static_component()
{
    static const u64 id = utility::FamilyGen::type<C>();

    auto it = m_static_component_index->find(id);

    if (it == m_static_component_index->end()) {
        it = m_static_component_index->emplace(id, component<C>()).first;
    }

    return it->second;
}

template <Component C>
std::optional<Entity> World::get_static_component() const
{
    static const u64 id = utility::FamilyGen::type<C>();

    const auto it = m_static_component_index->find(id);

    if (it == m_static_component_index->end()) {
        return std::nullopt;
    } else {
        return { it->second };
    }
}

template <Component C>
void World::set(Entity entity, C&& data)
{
    emplace<C>(entity, std::forward<C>(data));
}

template <Component C, class... Args>
    requires std::constructible_from<C, Args...>
void World::emplace(Entity entity, Args&&... args)
{
    mutate(entity, static_component<C>(), [&](void* data, bool is_new) {
        C* dest_ptr = static_cast<C*>(data);

        if (!is_new) {
            dest_ptr->~C();
        }

        new (dest_ptr) C(std::forward<Args>(args)...);
    });
}

template <Component C>
const C* World::get(Entity entity) const
{
    if (auto comp = get_static_component<C>()) {
        return static_cast<const C*>(get(entity, *comp));
    }

    return nullptr;
}

template <Component C, Component... Cs>
void World::remove(Entity entity)
{
    if (auto comp = get_static_component<C>()) {
        remove(entity, *comp);
    }

    // recursively remove the remaining components
    if constexpr (sizeof...(Cs) > 0) {
        remove<Cs...>(entity);
    }
}

template <Component... Cs>
Query<Cs...> World::query()
{
    return Query(*this).all<Cs...>();
}

template <Component... Cs>
SystemBuilder<Cs...> World::system()
{
    return SystemBuilder<>(*this).all<Cs...>();
}

template <Component... Cs>
template <Component... Css>
Query<Cs..., Css...> Query<Cs...>::all()
{
    return IGE_TODO(Query<Cs..., Css...>);
}

template <Component... Cs>
template <std::invocable<Cs&...> F>
void Query<Cs...>::each(F&& f)
{
    IGE_TODO();
}

template <Component... Cs>
Entity World::spawn(Cs&&... components)
{
    return IGE_TODO(Entity);
}

template <Plugin P>
bool World::load()
{
    // TODO: skip if plugin is already loaded
    spawn(P(*this));

    return true;
}

}
