#ifndef CEB37796_B8DF_47FF_8BEC_33592AA50536
#define CEB37796_B8DF_47FF_8BEC_33592AA50536

#include "InputRegistry.hpp"
#include <unordered_map>

namespace ige {
namespace plugin {
    namespace input {
        using ControllerId = std::size_t;

        enum class ControllerAxe { LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y };

        enum class ControllerButton {
            A,
            B,
            X,
            Y,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            DPAD_UP,
            LeftShoulder,
            RightShoulder,
            LeftStick,
            RightStick,
            Back,
            Start,
            Guide,
        };

        enum class ControllerEventType {
            CONNECTION,
            DISCONNECTION,
            JOYSTICK,
            BUTTON
        };

        struct ControllerEventJoystick {
            ControllerAxe axe;
            float value;
        };

        struct ControllerEventButton {
            ControllerButton button;
            InputRegistryState state;
        };

        struct ControllerEvent {
            ControllerEventType type;
            ControllerId id;
            union {
                ControllerEventJoystick joystick;
                ControllerEventButton button;
            };
        };

        class Controller : public InputRegistry<ControllerButton> {

        public:
            void set_axe_value(ControllerAxe axe, float value);

            void get_axe_value(ControllerAxe axe);

            void handle_controller_event(ControllerEvent event);

        private:
            std::unordered_map<ControllerAxe, float> m_axes;
        };
    }
}
}

#endif /* CEB37796_B8DF_47FF_8BEC_33592AA50536 */
