#ifndef C7E38F23_9AFB_4804_8783_20087647C0CA
#define C7E38F23_9AFB_4804_8783_20087647C0CA

#include <unordered_set>

namespace ige {
namespace plugin {
    namespace input {

        enum class InputRegistryState { RELEASED, PRESSED, DOWN };

        template <typename T>
        class InputRegistry {
        private:
            std::unordered_set<T> m_pressed;
            std::unordered_set<T> m_down;
            std::unordered_set<T> m_released;

        public:
            void clear()
            {
                m_pressed.clear();
                m_down.clear();
                m_released.clear();
            }

            void set_state(T key, InputRegistryState state)
            {
                switch (state) {
                case InputRegistryState::PRESSED:
                    m_pressed.insert(key);
                    break;
                case InputRegistryState::DOWN:
                    m_down.insert(key);
                    break;
                case InputRegistryState::RELEASED:
                    m_released.insert(key);
                    break;
                }
            }

            bool is_pressed(const T& key) const
            {
                return m_pressed.contains(key);
            }

            bool is_down(const T& key) const
            {
                return m_down.contains(key);
            }

            bool is_released(const T& key) const
            {
                return m_released.contains(key);
            }
        };
    }
}
}

#endif /* C7E38F23_9AFB_4804_8783_20087647C0CA */
