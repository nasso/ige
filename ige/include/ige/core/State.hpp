
#ifndef CAE73E4C_C36A_49DD_BEE3_0F01F323198A
#define CAE73E4C_C36A_49DD_BEE3_0F01F323198A

#include <type_traits>

namespace ige {
namespace core {

    class App;

    class State {
    public:
        enum Status {
            STOPPED, /* The state is not running */
            RUNNING, /* The state is currently active */
            PAUSED, /* The state is not active */
        };

        virtual ~State() = default;

        /**
         * @brief If STOPPED, schedule the state to be RUNNING next update
         */
        void start();

        /**
         * @brief Schedule the state to be STOPPED next update
         */
        void stop();

        /**
         * @brief If RUNNING, schedule the state to be PAUSED next update
         */
        void pause();

        /**
         * @brief If PAUSED, schedule the state to be RUNNING next update
         */
        void resume();

        /**
         * @brief Update the state according to its current status
         */
        void update(App&);

        Status status() const;

    protected:
        virtual void on_start(App&);
        virtual void on_update(App&);
        virtual void on_pause(App&);
        virtual void on_resume(App&);
        virtual void on_stop(App&);

    private:
        Status m_status = STOPPED;
        Status m_next_status = STOPPED;
    };

}
}

#endif /* CAE73E4C_C36A_49DD_BEE3_0F01F323198A */
