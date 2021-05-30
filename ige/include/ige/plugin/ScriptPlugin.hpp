#ifndef D9D2D319_3B35_4ED4_A91E_D45FA25710D0
#define D9D2D319_3B35_4ED4_A91E_D45FA25710D0

#include "ige/core/App.hpp"
#include "ige/ecs/Component.hpp"
#include "ige/ecs/Entity.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/World.hpp"
#include <concepts>
#include <memory>
#include <vector>

namespace ige::plugin::script {

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

template <typename T>
concept Behaviour
    = std::move_constructible<T> && std::derived_from<T, CppBehaviour>;

class Scripts {
private:
    using Behaviours = std::vector<std::unique_ptr<CppBehaviour>>;

    Behaviours m_bhvrs;

public:
    template <Behaviour... Bs>
    static Scripts from(Bs&&... bhvrs)
    {
        Scripts scripts;
        scripts.m_bhvrs.reserve(sizeof...(Bs));
        (scripts.m_bhvrs.push_back(
             std::make_unique<Bs>(std::forward<Bs>(bhvrs))),
         ...);
        return scripts;
    }

    void run_all(ecs::World&, ecs::EntityId);
};

class ScriptPlugin : public core::App::Plugin {
public:
    void plug(core::App::Builder&) const override;
};

}

#endif /* D9D2D319_3B35_4ED4_A91E_D45FA25710D0 */
