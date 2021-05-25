#ifndef BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E
#define BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E

#include "InputRegistry.hpp"
#include <unordered_map>

#include <glm/vec2.hpp>
; // TODO: https://bit.ly/3hhMJ58

namespace ige::plugin::input {

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE,
};

enum class MouseEventType {
    MOUSE_MOVE,
    BUTTON,
    SCROLL,
};

struct MouseMoveEvent {
    float x;
    float y;
};

struct MouseButtonEvent {
    MouseButton button;
    InputRegistryState state;
};

struct MouseScrollEvent {
    float x;
    float y;
};

struct MouseEvent {
    MouseEventType type;
    union {
        MouseMoveEvent pos;
        MouseButtonEvent button;
        MouseScrollEvent scroll;
    };
};

class Mouse : public InputRegistry<MouseButton> {
public:
    void clear() override;

    const glm::vec2& get_position() const;
    const glm::vec2& get_movement() const;
    const glm::vec2& get_scroll() const;

    void handle_mouse_event(const MouseEvent& event);

private:
    void handle_event(const MouseButtonEvent& event);
    void handle_event(const MouseMoveEvent& event);
    void handle_event(const MouseScrollEvent& event);

    glm::vec2 m_pos { 0.0f };
    glm::vec2 m_movement { 0.0f };
    glm::vec2 m_scroll { 0.0f };
};

}

#endif /* BC3DB51A_3CE0_4AE3_9AA4_E4D74D86D16E */
