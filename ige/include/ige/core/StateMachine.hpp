/*
** EPITECH PROJECT, 2021
** ige
** File description:
** StateMachine
*/

#ifndef A36D202D_24FB_4842_A4D0_ED897E9F7A2A
#define A36D202D_24FB_4842_A4D0_ED897E9F7A2A

#include "State.hpp"
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>
#include <vector>

namespace ige {
namespace core {

    class App;

    class StateMachine {
    private:
        std::vector<std::unique_ptr<State>> m_states;
        std::vector<std::unique_ptr<State>> m_stopping;

        void remove_current_state();

    public:
        /**
         * @brief Replace the current state with another one.
         *
         * @tparam S Type of the new state
         * @param args Constructor arguments for the new state
         */
        template <typename S, typename... Args,
            typename = std::enable_if_t<traits::is_state<S>::value
                && std::is_constructible<S, Args...>::value>>
        S& switch_to(Args&&... args)
        {
            if (auto cur = current()) {
                cur->get().stop();
            }

            remove_current_state();

            m_states.push_back(
                std::make_unique<S>(std::forward<Args>(args)...));

            auto& cur = current()->get();
            cur.start();
            return static_cast<S&>(cur);
        }

        /**
         * @brief Push a new state on top of the current one.
         *
         * @tparam S Type of the new state
         * @param args Constructor arguments for the new state
         */
        template <typename S, typename... Args,
            typename = std::enable_if_t<traits::is_state<S>::value
                && std::is_constructible<S, Args...>::value>>
        S& push(Args&&... args)
        {
            if (auto cur = current()) {
                cur->get().pause();
            }

            m_states.push_back(
                std::make_unique<S>(std::forward<Args>(args)...));

            auto& cur = current()->get();
            cur.start();
            return static_cast<S&>(cur);
        }

        void pop();
        void quit();

        void update(App&);
        bool is_running() const;

        std::optional<std::reference_wrapper<State>> current();
        std::optional<std::reference_wrapper<const State>> current() const;
    };

}
}

#endif /* A36D202D_24FB_4842_A4D0_ED897E9F7A2A */