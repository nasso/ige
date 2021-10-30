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
 * @tparam Cs A list of components to be added to the new entity.
 * @return Entity ID of the new entity.
 */
template <Component... Cs>
Entity World::spawn(Cs&&...)
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
 */
template <Component... Cs>
void World::add(Entity, Cs&&...)
{
}

/**
 * @brief Remove components from an entity.
 *
 * If any of the components are not present on the entity, no action is taken.
 */
template <Component... Cs>
void World::remove(Entity, Cs&&...)
{
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
const C* World::get(Entity) const
{
    return IGE_TODO(const C*);
}

template <Component... Cs>
void World::set(Entity, Cs&&...)
{
}

#if IGE_WORLD_RESOURCE_API
/**
 * @brief Get a constant pointer to a resource.
 *
 * If no resource of the specified type exists, a null pointer is returned.
 *
 * @tparam C The resource name.
 * @return const C* A pointer to the resource.
 */
template <Component C>
const C* World::get() const
{
    return IGE_TODO(const C*);
}
#endif

}
