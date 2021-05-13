#ifndef CAE73E4C_C36A_49DD_BEE3_0F01F323198A
#define CAE73E4C_C36A_49DD_BEE3_0F01F323198A

#include <type_traits>

namespace ige {
namespace core {

    class App;

    class State {
    public:
        virtual ~State() = default;

        virtual void on_start(App&);
        virtual void on_update(App&);
        virtual void on_pause(App&);
        virtual void on_resume(App&);
        virtual void on_stop(App&);
    };

}
}

#endif /* CAE73E4C_C36A_49DD_BEE3_0F01F323198A */
