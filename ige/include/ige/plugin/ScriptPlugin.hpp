#ifndef D9D2D319_3B35_4ED4_A91E_D45FA25710D0
#define D9D2D319_3B35_4ED4_A91E_D45FA25710D0

#include "ige/core/App.hpp"
#include "ige/core/TypeId.hpp"
#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/World.hpp"
#include "ige/plugin/UiPlugin.hpp"
#include <concepts>
#include <memory>
#include <unordered_map>

namespace ige::plugin::script {

class CppBehaviour;

template <typename T>
concept Behaviour
    = std::move_constructible<T> && std::derived_from<T, CppBehaviour>;

class CppBehaviour {
private:
    struct Context {
        ecs::World& world;
        ecs::EntityId entity;
    };

    std::optional<Context> m_context;

protected:
    ecs::EntityId entity() const;
    ecs::World& world() const;

    template <ecs::Resource R>
    R* get_resource()
    {
        return world().get<R>();
    }

    template <ecs::Resource R>
    const R* get_resource() const
    {
        return world().get<R>();
    }

    template <ecs::Resource R>
    R* add_resource(R res)
    {
        return &world().insert<R>(std::move(res));
    }

    template <ecs::Resource R, typename... Args>
    requires std::constructible_from<R, Args...> R*
    emplace_resource(Args&&... args)
    {
        return &world().emplace<R>(std::forward<Args>(args)...);
    }

    template <ecs::Resource R, typename... Args>
    requires std::constructible_from<R, Args...> R*
    get_or_emplace_resource(Args&&... args)
    {
        return &world().get_or_emplace<R>(std::forward<Args>(args)...);
    }

    template <ecs::Component C>
    C* get_component()
    {
        return world().get_component<C>(entity());
    }

    template <ecs::Component C>
    const C* get_component() const
    {
        return world().get_component<C>(entity());
    }

    template <ecs::Component C>
    C* add_component(C comp)
    {
        return &world().add_component<C>(entity(), std::move(comp));
    }

    template <ecs::Component C, typename... Args>
    requires std::constructible_from<C, Args...> C*
    emplace_component(Args&&... args)
    {
        return &world().emplace_component<C>(
            entity(), std::forward<Args>(args)...);
    }

    template <ecs::Component C, typename... Args>
    requires std::constructible_from<C, Args...> C*
    get_or_emplace_component(Args&&... args)
    {
        return &world().get_or_emplace_component<C>(
            entity(), std::forward<Args>(args)...);
    }

    template <Behaviour B>
    B* get_script();

    template <Behaviour B>
    const B* get_script() const;

public:
    virtual ~CppBehaviour() = default;

    bool set_context(ecs::World&, ecs::EntityId);

    /**
     * @brief Called on the first frame on which this script is active.
     */
    virtual void on_start();

    /**
     * @brief Called every frame.
     */
    virtual void update();

    /**
     * @brief Like `update`, but called on a fixed time interval.
     */
    virtual void tick();
};

class Scripts {
private:
    using BehaviourContainer
        = std::unordered_map<core::TypeId, std::unique_ptr<CppBehaviour>>;

    BehaviourContainer m_bhvrs;

public:
    template <Behaviour... Bs>
    static Scripts from(Bs&&... bhvrs)
    {
        Scripts scripts;
        (scripts.add(std::forward<Bs>(bhvrs)), ...);
        return scripts;
    }

    template <typename E, Behaviour B>
    static ui::EventTarget::Callback<E> event(void (B::*method)(const E&))
    {
        return [=](ecs::World& world, const ecs::EntityId& entity,
                   const E& event) {
            if (auto scripts = world.get_component<Scripts>(entity)) {
                if (auto behaviour = scripts->get<B>()) {
                    (behaviour->*method)(event);
                }
            }
        };
    }

    template <Behaviour B>
    void add(B bhvr)
    {
        m_bhvrs.insert({
            core::type_id<B>(),
            std::make_unique<B>(std::move(bhvr)),
        });
    }

    template <Behaviour B>
    B* get()
    {
        auto iter = m_bhvrs.find(core::type_id<B>());

        if (iter != m_bhvrs.end()) {
            return reinterpret_cast<B*>(iter->second.get());
        } else {
            return nullptr;
        }
    }

    template <Behaviour B>
    const B* get() const
    {
        auto iter = m_bhvrs.find(core::type_id<B>());

        if (iter != m_bhvrs.end()) {
            return reinterpret_cast<const B*>(iter->second.get());
        } else {
            return nullptr;
        }
    }

    void run_all(ecs::World&, ecs::EntityId);
};

template <Behaviour B>
B* CppBehaviour::get_script()
{
    return get_component<Scripts>()->get<B>();
}

template <Behaviour B>
const B* CppBehaviour::get_script() const
{
    return get_component<Scripts>()->get<B>();
}

class ScriptPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* D9D2D319_3B35_4ED4_A91E_D45FA25710D0 */
