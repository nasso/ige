#ifndef BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E
#define BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E

#include "InputRegistry.hpp"
#include <unordered_map>

namespace ige {
namespace plugin {

    namespace input {
        enum class MouseButton { LEFT, RIGHT, MIDDLE };

        enum class MouseEventType { MOUSE_MOVE, BUTTON, SCROLL };

        struct MouseMoveEvent {
            float x;
            float y;
        };

        struct MouseButtonEvent {
            MouseButton button;
            InputRegistryState state;
        };

        struct MouseEvent {
            MouseEventType type;
            union {
                MouseMoveEvent pos;
                MouseButtonEvent button;
            };
        };

        class Mouse : public InputRegistry<MouseButton> {
        public:
            struct MousePos {
                float x;
                float y;
            };

            void set_position(float x, float y);
            MousePos get_position() const;

            void handle_mouse_event(const MouseEvent& event);

        private:
            void handle_button_event(const MouseButtonEvent& event);
            void handle_position_event(const MouseMoveEvent& event);

            float m_xpos = 0;
            float m_ypos = 0;
        };
    }
}
}

#endif /* BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E */
