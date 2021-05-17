#ifndef BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E
#define BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E

#include "InputRegistry.hpp"
#include <unordered_map>

namespace ige {
namespace plugin {

    namespace input {
        enum class MouseButton { LEFT, RIGHT, MIDDLE };

        enum class MouseEventType { MOUSEPOS, BUTTON, SCROLL };

        struct MouseEventPos {
            double xpos;
            double ypos;
        };

        struct MouseEventButton {
            MouseButton button;
            InputRegistryState state;
        };

        struct MouseEvent {
            MouseEventType type;
            union {
                MouseEventPos pos;
                MouseEventButton button;
            };
        };

        class Mouse : public InputRegistry<MouseButton> {
        public:
            struct MousePos {
                double xpos;
                double ypos;
            };

            void set_pos(double xpos, double ypox);
            MousePos get_pos() const;

            void handle_mouse_event(MouseEvent event);

        protected:
        private:
            void handle_button_event(MouseEventButton event);
            void handle_position_event(MouseEventPos event);

            double m_xpos = 0;
            double m_ypos = 0;
        };
    }
}
}

#endif /* BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E */
