#include "World.hpp"
#include "ige/utility/Control.hpp"
#include <functional>

namespace ige::ecs {

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

/**
 * @brief Spawn a new entity.
 *
 * @param components A list of components to be added to the new entity.
 * @return Entity ID of the new entity.
 */
template <Component... Cs>
Entity World::spawn(Cs&&... components)
{
    return IGE_TODO(Entity);
}

/**
 * @brief Load a plugin.
 *
 * If the plugin was already loaded to this world, no action is taken.
 *
 * @tparam P The plugin type.
 * @return bool true if the plugin was loaded, false if it was already.
 */
template <Plugin P>
bool World::load()
{
    // TODO: skip if plugin is already loaded
    spawn(P(*this));

    return true;
}

/**
 * @brief Add components to an entity.
 *
 * If any of the components are already present on the entity, they are
 * replaced.
 *
 * @param entity The entity to add the components to.
 * @param components The components to add.
 */
template <Component... Cs>
void World::add(Entity entity, Cs&&... components)
{
    IGE_TODO();
}

/**
 * @brief Remove components from an entity.
 *
 * If any of the components are not present on the entity, no action is taken.
 *
 * @param entity The entity to remove the components from.
 * @param components The components to remove.
 */
template <Component... Cs>
void World::remove(Entity entity, Cs&&... components)
{
    IGE_TODO();
}

/**
 * @brief Get a constant pointer to a component attached to the given entity.
 *
 * If no component of the specified type exists, a null pointer is returned.
 *
 * @param entity The entity.
 * @return const C* A pointer to the component.
 */
template <Component C>
const C* World::get(Entity entity) const
{
    return IGE_TODO(const C*);
}

/**
 * @brief Set the value of one or more components attached to the given entity.
 *
 * If any component is missing from the entity, it is added.
 *
 * @tparam Cs The component types.
 * @param entity The entity.
 * @param components The component values to set.
 */
template <Component... Cs>
void World::set(Entity entity, Cs&&... components)
{
    IGE_TODO();
}

/**
 * @brief Get a constant pointer to a resource.
 *
 * If no resource of the specified type exists, a null pointer is returned.
 *
 * @tparam C The resource type.
 * @return const C* A pointer to the resource.
 */
template <Component C>
const C* World::get() const
{
    return IGE_TODO(const C*);
}

}
