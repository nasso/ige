#ifndef A234BCD0_7A4F_4760_9B0C_9FCEA88EB91C
#define A234BCD0_7A4F_4760_9B0C_9FCEA88EB91C

#include "ige/core/App.hpp"
#include "ige/plugin/input/Controller.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace ige {
namespace plugin {

    class GLFWInputPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
    };

}
}

#endif /* A234BCD0_7A4F_4760_9B0C_9FCEA88EB91C */
