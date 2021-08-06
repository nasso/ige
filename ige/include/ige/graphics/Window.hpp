#ifndef AF975F15_4BC7_403C_9C58_A18C21F4A20A
#define AF975F15_4BC7_403C_9C58_A18C21F4A20A

#include "ige/utility/Types.hpp"
#include <string>

namespace ige::graphics {

/**
 * @brief Component representing a window.
 */
struct Window {
    std::string title;
    u32 width = 0;
    u32 height = 0;
};

}

#endif /* AF975F15_4BC7_403C_9C58_A18C21F4A20A */
