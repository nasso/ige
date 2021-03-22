/*
** EPITECH PROJECT, 2021
** un
** File description:
** App
*/

#ifndef F7EA1363_932A_4910_8D9B_4CA25912EBED
#define F7EA1363_932A_4910_8D9B_4CA25912EBED

#include "rtl/Option.hpp"
#include "un/core/State.hpp"
#include "un/core/StateMachine.hpp"
#include "un/ecs/Schedule.hpp"
#include "un/ecs/System.hpp"
#include "un/ecs/World.hpp"
#include <vector>

namespace un {
namespace core {

    class App {
    private:
        core::StateMachine m_state_machine;
        ecs::World m_world;
        rtl::Option<ecs::Schedule> m_startup;
        rtl::Option<ecs::Schedule> m_update;
        rtl::Option<ecs::Schedule> m_cleanup;

        template <typename S, typename... Args>
        static constexpr bool can_make_state = core::traits::is_state_v<S>&&
            std::is_constructible<S, Args...>::value;

        App(rtl::Option<ecs::Schedule> start, rtl::Option<ecs::Schedule> update,
            rtl::Option<ecs::Schedule> clean)
            : m_startup(std::move(start))
            , m_update(std::move(update))
            , m_cleanup(std::move(clean))
        {
        }

    public:
        class Builder {
        private:
            rtl::Option<ecs::Schedule> m_startup;
            rtl::Option<ecs::Schedule> m_update;
            rtl::Option<ecs::Schedule> m_cleanup;

        public:
            void on_start(ecs::Schedule);
            void on_update(ecs::Schedule);
            void on_stop(ecs::Schedule);

            template <typename S, typename... Args>
            std::enable_if_t<can_make_state<S, Args...>>
            run(Args&&... args)
            {
                App app(m_startup.take(), m_update.take(), m_cleanup.take());

                app.state_machine().push<S>(std::forward<Args>(args)...);
                app.run();
            }
        };

        core::StateMachine& state_machine();
        const core::StateMachine& state_machine() const;

        void run();
        void quit();
    };

}
}

#endif /* F7EA1363_932A_4910_8D9B_4CA25912EBED */
