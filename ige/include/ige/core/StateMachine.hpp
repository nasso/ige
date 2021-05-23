#ifndef A36D202D_24FB_4842_A4D0_ED897E9F7A2A
#define A36D202D_24FB_4842_A4D0_ED897E9F7A2A

#include "State.hpp"
#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <queue>
#include <type_traits>
#include <vector>

namespace ige {
namespace core {

    class App;

    class StateMachine {
    private:
        enum class CommandType {
            PUSH,
            POP,
            SWITCH,
            QUIT,
        };

        struct Command {
            CommandType type;
            std::unique_ptr<State> state;
        };

        std::vector<std::unique_ptr<State>> m_states;
        std::queue<Command> m_command_queue;

        void perform(Command, App&);
        void queue(Command);

    public:
        /**
         * @brief Replace the current state with another one.
         *
         * @tparam S Type of the new state
         * @param args Constructor arguments for the new state
         */
        template <std::derived_from<State> S, typename... Args>
        requires std::constructible_from<S, Args...> S&
        switch_to(Args&&... args)
        {
            auto state = std::make_unique<S>(std::forward<Args>(args)...);
            auto& state_ref = *state;

            queue({
                CommandType::SWITCH,
                std::move(state),
            });

            return state_ref;
        }

        /**
         * @brief Push a new state on top of the current one.
         *
         * @tparam S Type of the new state
         * @param args Constructor arguments for the new state
         */
        template <std::derived_from<State> S, typename... Args>
        requires std::constructible_from<S, Args...> S& push(Args&&... args)
        {
            auto state = std::make_unique<S>(std::forward<Args>(args)...);
            auto& state_ref = *state;

            queue({
                CommandType::PUSH,
                std::move(state),
            });

            return state_ref;
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
