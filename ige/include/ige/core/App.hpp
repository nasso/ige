/*
** EPITECH PROJECT, 2021
** ige
** File description:
** App
*/

#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "ige/core/State.hpp"
#include "ige/core/StateMachine.hpp"
#include "ige/ecs/Schedule.hpp"
#include "ige/ecs/System.hpp"
#include "ige/ecs/World.hpp"
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

        template <typename S, typename... Args>
        static constexpr bool can_make_state = core::traits::is_state_v<S>&&
            std::is_constructible<S, Args...>::value;

        App(std::optional<ecs::Schedule> start,
            std::optional<ecs::Schedule> update,
            std::optional<ecs::Schedule> clean)
            : m_startup(std::move(start))
            , m_update(std::move(update))
            , m_cleanup(std::move(clean))
        {
        }

    public:
        class Builder {
        private:
            std::optional<ecs::Schedule> m_startup;
            std::optional<ecs::Schedule> m_update;
            std::optional<ecs::Schedule> m_cleanup;

        public:
            App::Builder& on_start(ecs::Schedule);
            App::Builder& on_update(ecs::Schedule);
            App::Builder& on_stop(ecs::Schedule);

            template <typename S, typename... Args>
            std::enable_if_t<can_make_state<S, Args...>> run(Args&&... args)
            {
                std::optional<ecs::Schedule> startup, update, cleanup;

                m_startup.swap(startup);
                m_update.swap(update);
                m_cleanup.swap(cleanup);

                App app(
                    std::move(startup), std::move(update), std::move(cleanup));

                app.state_machine().push<S>(std::forward<Args>(args)...);
                app.run();
            }
        };

        App() = default;

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
