#ifndef E2CD97D4_79A5_42B2_90CA_A083F9A9451D
#define E2CD97D4_79A5_42B2_90CA_A083F9A9451D

#include <concepts>
#include <functional>
#include <type_traits>

namespace ige::core {

class Any {
private:
    Any(void* data, std::function<void(void*)> dtor);

public:
    Any(const Any&) = delete;
    Any(Any&&);
    ~Any();

    template <typename T, typename... Args>
        requires std::constructible_from<T, Args...>
    static Any from(Args&&... args)
    {
        T* data = new T(std::forward<Args>(args)...);

        return Any(data, [](void* ptr) { delete static_cast<T*>(ptr); });
    }

    template <typename T>
    T& as()
    {
        return *static_cast<T*>(m_data);
    }

    template <typename T>
    const T& as() const
    {
        return *static_cast<T*>(m_data);
    }

private:
    void* m_data = nullptr;
    std::function<void(void*)> m_dtor;
};

}

#endif /* E2CD97D4_79A5_42B2_90CA_A083F9A9451D */
