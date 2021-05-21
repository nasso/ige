#ifndef F6293BA4_754A_4C51_89E4_62AC4B51F348
#define F6293BA4_754A_4C51_89E4_62AC4B51F348

#include "InputRegistry.hpp"

namespace ige {
namespace plugin {
    namespace input {
        enum class KeyboardKey {
            KEY_ESCAPE,
            KEY_BACK_SPACE,
            KEY_ARROW_RIGHT,
            KEY_ARROW_LEFT,
            KEY_ARROW_UP,
            KEY_ARROW_DOWN,
            KEY_SHIFT_LEFT,
            KEY_SHIFT_RIGHT,
            KEY_CTRL_LEFT,
            KEY_CTRL_RIGHT,
            KEY_ALT_LEFT,
            KEY_ALT_RIGHT,
            KEY_TAB,
            KEY_PAGE_UP,
            KEY_PAGE_DOWN,
            KEY_DELETE,
            KEY_INSERT,
            KEY_END_FILE,
            KEY_SPACE,
            KEY_F1,
            KEY_F2,
            KEY_F3,
            KEY_F4,
            KEY_F5,
            KEY_F6,
            KEY_F7,
            KEY_F8,
            KEY_F9,
            KEY_F10,
            KEY_F11,
            KEY_F12,
            KEY_A,
            KEY_B,
            KEY_C,
            KEY_D,
            KEY_E,
            KEY_F,
            KEY_G,
            KEY_H,
            KEY_I,
            KEY_J,
            KEY_K,
            KEY_L,
            KEY_M,
            KEY_N,
            KEY_O,
            KEY_P,
            KEY_Q,
            KEY_R,
            KEY_S,
            KEY_T,
            KEY_U,
            KEY_V,
            KEY_W,
            KEY_X,
            KEY_Y,
            KEY_Z,
            KEY_1,
            KEY_2,
            KEY_3,
            KEY_4,
            KEY_5,
            KEY_6,
            KEY_7,
            KEY_8,
            KEY_9,
            KEY_0,
        };

        struct KeyboardEvent {
            KeyboardKey key;
            InputRegistryState state;
        };

        class Keyboard : public InputRegistry<KeyboardKey> {
        public:
            void handle_keyboard_event(KeyboardEvent event);
        };
    }
}
}

#endif /* F6293BA4_754A_4C51_89E4_62AC4B51F348 */
