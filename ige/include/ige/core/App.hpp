#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "ige/core/State.hpp"
#include "ige/core/StateMachine.hpp"
#include "ige/ecs/Resources.hpp"
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
#include <concepts>
#include <optional>
#include <utility>
#include <vector>

namespace ige {
namespace core {

    class App {
    private:
        core::StateMachine m_state_machine;
        ecs::World m_world;
        std::optional<ecs::Schedule> m_startup;
        std::optional<ecs::Schedule> m_update;
        std::optional<ecs::Schedule> m_cleanup;

    public:
        class Builder {
        private:
            std::optional<ecs::Schedule> m_startup;
            std::optional<ecs::Schedule> m_update;
            std::optional<ecs::Schedule> m_cleanup;
            ecs::Resources m_res;

        public:
            App::Builder& on_start(ecs::Schedule);
            App::Builder& on_update(ecs::Schedule);
            App::Builder& on_stop(ecs::Schedule);

            template <ecs::Resource R>
            App::Builder& insert(R res)
            {
                m_res.insert(res);
                return *this;
            }

            template <ecs::Resource R, typename... Args>
            requires std::constructible_from<R, Args...> App::Builder& emplace(
                Args&&... args)
            {
                m_res.set<R>(std::forward<Args>(args)...);
                return *this;
            }

            template <std::derived_from<State> S, typename... Args>
                requires std::constructible_from<S, Args...>
            void run(Args&&... args)
            {
                App app(std::move(m_res));
                m_res = ecs::Resources();

                app.m_startup.swap(m_startup);
                app.m_update.swap(m_update);
                app.m_cleanup.swap(m_cleanup);

                app.state_machine().push<S>(std::forward<Args>(args)...);
                app.run();
            }
        };

        App(ecs::Resources = {});

        ecs::World& world();
        const ecs::World& world() const;

        core::StateMachine& state_machine();
        const core::StateMachine& state_machine() const;

        void run();
        void quit();
    };

}
}

#endif /* F7EA1363_932A_4910_8D9B_4CA25912EBED */
