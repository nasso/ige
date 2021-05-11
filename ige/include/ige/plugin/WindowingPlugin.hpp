#ifndef A68DBC66_D5CF_4EE6_B409_AFF8B6AE00F6
#define A68DBC66_D5CF_4EE6_B409_AFF8B6AE00F6

#include "ige/core/App.hpp"
#include <cstdint>

namespace ige {
namespace plugin {

    struct WindowSettings {
        std::string title;
        std::uint32_t width;
        std::uint32_t height;
    };

    struct WindowInfo {
        std::uint32_t width;
        std::uint32_t height;
    };

    enum class WindowEventKind {
        WindowClose,
    };

    struct WindowEvent {
        WindowEventKind kind;
    };

    class WindowingPlugin : public core::App::Plugin {
    public:
        void plug(core::App::Builder&) const override;
    };

}
}

#endif /* A68DBC66_D5CF_4EE6_B409_AFF8B6AE00F6 */
