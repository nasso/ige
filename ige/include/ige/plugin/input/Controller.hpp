#ifndef CEB37796_B8DF_47FF_8BEC_33592AA50536
#define CEB37796_B8DF_47FF_8BEC_33592AA50536

#include "InputRegistry.hpp"
#include <unordered_map>

namespace ige::plugin::input {

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
    LEFT_BUMPER,
    RIGHT_BUMPER,
    LEFT_THUMB,
    RIGHT_THUMB,
    BACK,
    START,
    GUIDE,
};

enum class ControllerEventType {
    CONNECTION,
    DISCONNECTION,
};

struct ControllerEvent {
    ControllerEventType type;
    ControllerId id;
};

class Controller : public InputRegistry<ControllerButton> {
public:
    Controller(const ControllerId& id);

    ControllerId id() const;
    void set_axis_value(ControllerAxis axis, float value);
    float get_axis_value(ControllerAxis axis);

private:
    ControllerId m_id;
    std::unordered_map<ControllerAxis, float> m_axes;
};

}

#endif /* CEB37796_B8DF_47FF_8BEC_33592AA50536 */
