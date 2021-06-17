#ifndef B7EA021F_3728_4158_AD5D_6BB02383B0D5
#define B7EA021F_3728_4158_AD5D_6BB02383B0D5

#include "Controller.hpp"
#include "Keyboard.hpp"
#include "Mouse.hpp"
#include <optional>
#include <unordered_map>
#include <vector>

namespace ige::plugin::input {

enum class InputEventType {
    KEYBOARD,
    MOUSE,
};

struct InputEvent {
    InputEventType type;
    union {
        KeyboardEvent keyboard;
        MouseEvent mouse;
    };
};

class InputManager {
public:
    using ControllerMap = std::unordered_map<ControllerId, Controller>;
    using ControllerMapIterator = ControllerMap::iterator;
    using ControllerMapConstIterator = ControllerMap::const_iterator;

    Keyboard& keyboard();
    const Keyboard& keyboard() const;
    Mouse& mouse();
    const Mouse& mouse() const;

    Controller& add_controller(const ControllerId& id);

    std::pair<ControllerMapIterator, ControllerMapIterator> controllers();
    std::pair<ControllerMapConstIterator, ControllerMapConstIterator>
    controllers() const;

    Controller* controller(const ControllerId& id);
    const Controller* controller(const ControllerId& id) const;

    void reset();
    void push_event(const InputEvent& wld);

private:
    std::unordered_map<ControllerId, Controller> m_controllers;
    Keyboard m_keyboard;
    Mouse m_mouse;
};

}

#endif /* B7EA021F_3728_4158_AD5D_6BB02383B0D5 */
