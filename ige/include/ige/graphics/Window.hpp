#ifndef AF975F15_4BC7_403C_9C58_A18C21F4A20A
#define AF975F15_4BC7_403C_9C58_A18C21F4A20A

#include "ige/core/Types.hpp"
#include <string>

namespace ige::graphics {

struct Window {
    std::string title;
    u32 width;
    u32 height;
};

struct WindowPlugin {
    WindowPlugin(core::World&);
};

}

#endif /* AF975F15_4BC7_403C_9C58_A18C21F4A20A */
