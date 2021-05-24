#ifndef A0057E91_19D4_4C57_A362_8A5B53D85BDE
#define A0057E91_19D4_4C57_A362_8A5B53D85BDE

#include "ige/ecs/MapStorage.hpp"
#include "ige/ecs/Resources.hpp"
#include <concepts>

namespace ige::ecs {

namespace detail {

    template <typename S, typename C>
    concept StorageFor = requires(S& strg, const S& cstrg, C comp)
    {
        // void set(id, ...)
        strg.set(std::size_t(0), std::move(comp));

        // C* get(id)
        {
            strg.get(std::size_t(0))
            } -> std::same_as<C*>;

        // const C* get(id) const
        {
            cstrg.get(std::size_t(0))
            } -> std::same_as<const C*>;

        // std::optional<C> remove(id)
        {
            strg.remove(std::size_t(0))
            } -> std::same_as<std::optional<C>>;

        // iterator returns indices
        {
            std::size_t(strg.begin()->first)
            } -> std::same_as<std::size_t>;

        // iterator returns components
        {
            strg.begin()->second
            } -> std::same_as<C&>;

        // iterator returns indices
        {
            std::size_t(strg.end()->first)
            } -> std::same_as<std::size_t>;

        // iterator returns components
        {
            strg.end()->second
            } -> std::same_as<C&>;
    };

    template <typename T>
    concept HasStorage = requires
    {
        typename T::Storage;
    };

    template <typename C>
    struct ComponentStorage {
        using Type = MapStorage<C>;
    };

    template <typename C>
        requires HasStorage<C>
    struct ComponentStorage<C> {
        using Type = typename C::Storage;
    };

}

template <typename C>
concept Component = requires
{
    requires Resource<C>;
    requires detail::StorageFor<typename detail::ComponentStorage<C>::Type, C>;
};

template <Component C>
using StorageOf = typename detail::ComponentStorage<C>::Type;

}

#endif /* A0057E91_19D4_4C57_A362_8A5B53D85BDE */
