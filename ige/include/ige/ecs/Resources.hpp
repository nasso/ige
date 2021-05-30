#ifndef A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6
#define A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6

#include "ige/core/Any.hpp"
#include "ige/core/TypeId.hpp"
#include <concepts>
#include <functional>
#include <optional>
#include <type_traits>
#include <utility>

namespace ige::ecs {

template <typename R>
concept Resource = std::movable<R> && std::same_as<std::decay_t<R>, R>;

class Resources {
public:
    template <Resource R>
    R& insert(R res)
    {
        core::TypeId id = core::type_id<R>();

        return set_any(id, core::Any::from<R>(std::move(res))).template as<R>();
    }

    template <Resource R, typename... Args>
    requires std::constructible_from<R, Args...> R& emplace(Args&&... args)
    {
        core::TypeId id = core::type_id<R>();

        return set_any(id, core::Any::from<R>(std::forward<Args>(args)...))
            .template as<R>();
    }

    template <Resource R, typename... Args>
    requires std::constructible_from<R, Args...> R&
    get_or_emplace(Args&&... args)
    {
        if (auto val = get<R>()) {
            return *val;
        } else {
            return emplace<R>(std::forward<Args>(args)...);
        }
    }

    template <Resource R>
    R* get()
    {
        core::TypeId id = core::type_id<R>();

        if (auto any = get_any(id)) {
            return &any->template as<R>();
        } else {
            return nullptr;
        }
    }

    template <Resource R>
    const R* get() const
    {
        core::TypeId id = core::type_id<R>();

        if (auto any = get_any(id)) {
            return &any->template as<R>();
        } else {
            return nullptr;
        }
    }

    template <Resource R>
    std::optional<R> remove()
    {
        core::TypeId id = core::type_id<R>();

        if (auto any = remove_any(id)) {
            return { std::move(any->template as<R>()) };
        } else {
            return std::nullopt;
        }
    }

private:
    std::unordered_map<core::TypeId, core::Any> m_resources;

    core::Any& set_any(core::TypeId id, core::Any any)
    {
        m_resources.erase(id);
        m_resources.insert(std::make_pair(id, std::move(any)));
        return m_resources.at(id);
    }

    core::Any* get_any(core::TypeId id)
    {
        auto it = m_resources.find(id);

        if (it != m_resources.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    const core::Any* get_any(core::TypeId id) const
    {
        auto it = m_resources.find(id);

        if (it != m_resources.end()) {
            return &it->second;
        } else {
            return nullptr;
        }
    }

    std::optional<core::Any> remove_any(core::TypeId id)
    {
        auto it = m_resources.find(id);

        if (it != m_resources.end()) {
            auto any = std::move(it->second);
            m_resources.erase(id);
            return { std::move(any) };
        } else {
            return std::nullopt;
        }
    }
};

}

#endif /* A0B328C5_412E_4F22_A2FB_18A5C0CBD2F6 */
