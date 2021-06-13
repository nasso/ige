#ifndef C7E38F23_9AFB_4804_8783_20087647C0CA
#define C7E38F23_9AFB_4804_8783_20087647C0CA

#include <unordered_set>

namespace ige::plugin::input {

enum class InputRegistryState {
    RELEASED,
    PRESSED,
};

template <typename T>
class InputRegistry {
private:
    std::unordered_set<T> m_pressed;
    std::unordered_set<T> m_down;
    std::unordered_set<T> m_released;

public:
    virtual ~InputRegistry() = default;

    virtual void clear()
    {
        m_pressed.clear();
        m_released.clear();
        m_down.clear();
    }

    void set_state(T key, InputRegistryState state)
    {
        switch (state) {
        case InputRegistryState::PRESSED:
            m_pressed.insert(key);
            m_down.insert(key);
            break;
        case InputRegistryState::RELEASED:
            m_down.erase(key);
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

    std::vector<T> get_all_down() const
    {
        return m_down;
    }

    std::vector<T> get_all_pressed() const
    {
        return m_pressed;
    }

    std::vector<T> get_all_released() const
    {
        return m_released;
    }
};

}

#endif /* C7E38F23_9AFB_4804_8783_20087647C0CA */
