#ifndef B7EA021F_3728_4158_AD5D_6BB02383B0D5
#define B7EA021F_3728_4158_AD5D_6BB02383B0D5

#include "Keyboard.hpp"
#include "Mouse.hpp"
#include "ige/core/EventChannel.hpp"
#include "ige/ecs/World.hpp"
#include <functional>
#include <optional>
#include <string>
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
    Keyboard& keyboard();
    const Keyboard& keyboard() const;
    Mouse& mouse();
    const Mouse& mouse() const;

    void reset();
    void push_event(const InputEvent& wld);

private:
    Keyboard m_keyboard;
    Mouse m_mouse;
};

}

#endif /* B7EA021F_3728_4158_AD5D_6BB02383B0D5 */
