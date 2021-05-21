#ifndef CEB37796_B8DF_47FF_8BEC_33592AA50536
#define CEB37796_B8DF_47FF_8BEC_33592AA50536

#include "InputRegistry.hpp"
#include <unordered_map>

namespace ige {
namespace plugin {
    namespace input {
        using ControllerId = std::size_t;

        enum class ControllerAxis {
            LEFT_X,
            LEFT_Y,
            RIGHT_X,
            RIGHT_Y,
            LEFT_TRIGGER,
            RIGHT_TRIGGER
        };

        enum class ControllerButton {
            A,
            B,
            X,
            Y,
            DPAD_DOWN,
            DPAD_LEFT,
            DPAD_RIGHT,
            DPAD_UP,
            LEFT_SHOULDER,
            RIGHT_SHOULDER,
            LEFT_STICK,
            RIGHT_STICK,
            BACK,
            START,
            GUIDE,
        };

        enum class ControllerEventType {
            CONNECTION,
            DISCONNECTION,
            JOYSTICK,
            BUTTON
        };

        struct ControllerEventJoystick {
            ControllerAxis axis;
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
            void set_axis_value(ControllerAxis axis, float value);

            float get_axis_value(ControllerAxis axis);

            void handle_controller_event(const ControllerEvent& event);

        private:
            std::unordered_map<ControllerAxis, float> m_axes;
        };
    }
}
}

#endif /* CEB37796_B8DF_47FF_8BEC_33592AA50536 */
