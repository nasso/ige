#include "World.hpp"
#include <functional>

namespace ige::ecs {

/**
 * @brief Spawn a new entity.
 *
 * @tparam Cs A list of components to be added to the new entity.
 * @return Entity ID of the new entity.
 */
template <Component... Cs>
Entity World::spawn(Cs&&...)
{
    // TODO
    return 0;
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
    // TODO
    return nullptr;
}

/**
 * @brief Get a mutable pointer to a resource.
 *
 * After this method is called, the resource is always assumed to have been
 * modified. If you aren't modifying the resource, you should use get() instead.
 *
 * If no resource of the specified type exists, a null pointer is returned.
 *
 * @tparam C The resource name.
 * @return C* A pointer to the resource.
 */
template <Component C>
C* World::get_mut()
{
    // TODO
    return nullptr;
}

/**
 * @brief Get a constant pointer to a component attached to the given entity.
 *
 * If no component of the specified type exists, a null pointer is returned.
 *
 * @param entity The entity.
 * @tparam C The component name.
 * @return const C* A pointer to the component.
 */
template <Component C>
const C* World::get(Entity) const
{
    // TODO
    return nullptr;
}

/**
 * @brief Get a mutable pointer to a component attached to the given entity.
 *
 * After this method is called, the component is always assumed to have been
 * modified. If you aren't modifying the component, you should use get()
 * instead.
 *
 * If no component of the specified type exists, a null pointer is returned.
 *
 * @param entity The entity.
 * @tparam C The component name.
 * @return C* A pointer to the component.
 */
template <Component C>
C* World::get_mut(Entity)
{
    // TODO
    return nullptr;
}

/**
 * @brief Add an "all" constraint to the system's query.
 *
 * The system will only be executed on entities which have all of the given
 * components attached.
 */
template <Component... Cs>
template <Component... Css>
SystemBuilder<Cs..., Css...> SystemBuilder<Cs...>::all()
{
    return SystemBuilder<Cs..., Css...>(m_world);
}

/**
 * @brief Define the function to be called when the system is executed.
 *
 * Any component passed by a non-const reference to the function will be
 * considered modified. Systems defined to be run when a component is modified
 * will be executed even if you did not actually modify it. For this reason, you
 * should prefer constant references whenever possible.
 */
template <Component... Cs>
template <std::invocable<Cs&...> F>
void SystemBuilder<Cs...>::each(F&&)
{
    // TODO
}

}
