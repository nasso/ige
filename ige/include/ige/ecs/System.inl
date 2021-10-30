namespace ige::ecs {

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
