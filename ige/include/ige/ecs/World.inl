#include "World.hpp"
#include "ige/utility/Assert.hpp"
#include "ige/utility/Control.hpp"
#include <concepts>
#include <functional>

namespace ige::ecs {

template <std::regular_invocable<void*> F>
void World::mutate(Entity entity, Entity component, F&& f)
{
    Record* record = get_record_mut(entity);

    IGE_ASSERT(record != nullptr, "\"entity\" is not alive");

    // ensure the entity has the component
    add_to_record(*record, component.id());

    // get the column index in which the component is stored
    usize column = get_component_column(record->family(), component);

    // get or create the table storing the components of the archetype
    Table& table = *touch_table(record->type_mut());

    // ensure the table has enough rows
    if (table.row_count() <= record->row()) {
        table.resize(record->row() + 1);
    }

    // mutate the data
    f(table.cell_mut(column, record->row()));

    // TODO: mark as modified
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
