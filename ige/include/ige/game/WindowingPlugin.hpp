#ifndef A68DBC66_D5CF_4EE6_B409_AFF8B6AE00F6
#define A68DBC66_D5CF_4EE6_B409_AFF8B6AE00F6

#include "ige/core/App.hpp"

namespace ige {
namespace game {

    struct WindowSettings {
        std::string title;
        unsigned int width;
        unsigned int height;
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
