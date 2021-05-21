#ifndef C7E38F23_9AFB_4804_8783_20087647C0CA
#define C7E38F23_9AFB_4804_8783_20087647C0CA

#include <unordered_map>

namespace ige {
namespace plugin {
    namespace input {

        enum class InputRegistryState { RELEASED, PRESSED, DOWN };

        template <typename T>
        class InputRegistry
            : private std::unordered_map<T, InputRegistryState> {
        public:
            void set_state(T key, InputRegistryState state)
            {
                this->insert_or_assign(key, state);
            }

            InputRegistryState get_state(const T& key) const
            {
                auto state = this->find(key);
                if (state == this->end()) {
                    return InputRegistryState::RELEASED;
                }
                return state->second;
            }

            bool is_pressed(const T& key) const
            {
                auto state = this->find(key);
                if (state == this->end()) {
                    return false;
                }
                return state->second == InputRegistryState::PRESSED;
            }

            bool is_down(const T& key) const
            {
                auto state = this->find(key);
                if (state == this->end()) {
                    return false;
                }
                return state->second == InputRegistryState::DOWN;
            }

            bool is_released(const T& key) const
            {
                auto state = this->find(key);
                if (state == this->end()) {
                    return true;
                }
                return state->second == InputRegistryState::RELEASED;
            }
        };
    }
}
}

#endif /* C7E38F23_9AFB_4804_8783_20087647C0CA */
