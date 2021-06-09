#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "ige/core/State.hpp"
#include "ige/core/StateMachine.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <concepts>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace ige::core {

class App {
private:
    core::StateMachine m_state_machine;
    ecs::World m_world;
    ecs::Schedule m_startup;
    ecs::Schedule m_update;
    ecs::Schedule m_cleanup;

public:
    class Builder;

    class Plugin {
    public:
        virtual ~Plugin() = default;

        virtual void plug(Builder&) const = 0;
    };

    class Builder {
    private:
        ecs::Schedule::Builder m_startup;
        ecs::Schedule::Builder m_update;
        ecs::Schedule::Builder m_cleanup;
        ecs::Resources m_res;

    public:
        App::Builder& add_plugin(const Plugin&);

        App::Builder& add_startup_system(std::unique_ptr<ecs::System>) &;
        App::Builder add_startup_system(std::unique_ptr<ecs::System>) &&;
        App::Builder& add_system(std::unique_ptr<ecs::System>) &;
        App::Builder add_system(std::unique_ptr<ecs::System>) &&;
        App::Builder& add_cleanup_system(std::unique_ptr<ecs::System>) &;
        App::Builder add_cleanup_system(std::unique_ptr<ecs::System>) &&;

        template <ecs::Resource R>
        App::Builder& insert(R res)
        {
            m_res.insert(res);
            return *this;
        }

        template <ecs::Resource R, typename... Args>
        requires std::constructible_from<R, Args...> App::Builder&
        emplace(Args&&... args)
        {
            m_res.emplace<R>(std::forward<Args>(args)...);
            return *this;
        }

        template <std::derived_from<State> S, typename... Args>
            requires std::constructible_from<S, Args...>
        void run(Args&&... args)
        {
            App app(
                std::move(m_res), m_startup.build(), m_update.build(),
                m_cleanup.build());
            m_res = ecs::Resources();

            app.state_machine().push<S>(std::forward<Args>(args)...);
            app.run();
        }
    };

    App(ecs::Resources, ecs::Schedule on_start, ecs::Schedule on_update,
        ecs::Schedule on_cleanup);
    App(ecs::Resources = {});

    ecs::World& world();
    const ecs::World& world() const;

    core::StateMachine& state_machine();
    const core::StateMachine& state_machine() const;

    void run();
    void quit();
};

}

#endif /* F7EA1363_932A_4910_8D9B_4CA25912EBED */
