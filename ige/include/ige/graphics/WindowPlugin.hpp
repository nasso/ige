#ifndef A2343F8A_C627_4776_814C_859306346CD2
#define A2343F8A_C627_4776_814C_859306346CD2

#include "ige/core/Platform.hpp"
#include "ige/core/World.hpp"

namespace ige::graphics {

struct IGE_API WindowPlugin {
    WindowPlugin(ige::core::World&);
    ~WindowPlugin();
};

}

#endif /* A2343F8A_C627_4776_814C_859306346CD2 */
