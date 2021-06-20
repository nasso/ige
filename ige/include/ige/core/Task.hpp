#ifndef B370887D_6420_42CE_BDAA_F50E9EAEF22C
#define B370887D_6420_42CE_BDAA_F50E9EAEF22C

#include <exception>
#include <functional>
#include <memory>
#include <optional>
#include <thread>

namespace ige::core {

struct BadTaskAccess : public std::exception {
public:
    const char* what() const noexcept;
};

template <typename T>
class Task {
    using SharedOpt = std::shared_ptr<std::optional<T>>;

public:
    Task()
        : m_value(std::make_shared<std::optional<T>>(std::nullopt))
    {
    }

    const T* operator->() const
    {
        return &**m_value;
    }

    T* operator->()
    {
        return &**m_value;
    }

    const T& operator*() const&
    {
        return **m_value;
    }

    T& operator*() &
    {
        return **m_value;
    }

    const T&& operator*() const&&
    {
        return std::move(**m_value);
    }

    T&& operator*() &&
    {
        return std::move(**m_value);
    }

    explicit operator bool() const noexcept
    {
        return has_value();
    }

    bool has_value() const noexcept
    {
        return m_value->has_value();
    }

    const T& value() const&
    {
        if (has_value()) {
            return **this;
        } else {
            throw BadTaskAccess();
        }
    }

    T& value() &
    {
        if (has_value()) {
            return **this;
        } else {
            throw BadTaskAccess();
        }
    }

    const T&& value() const&&
    {
        if (has_value()) {
            return **this;
        } else {
            throw BadTaskAccess();
        }
    }

    T&& value() &&
    {
        if (has_value()) {
            return **this;
        } else {
            throw BadTaskAccess();
        }
    }

    static Task<T> spawn(std::function<T()> job)
    {
        Task<T> task;

        std::thread([=, job = std::move(job)]() {
            task.m_value->emplace(job());
        }).detach();

        return task;
    }

private:
    std::shared_ptr<std::optional<T>> m_value;
};

}

#endif /* B370887D_6420_42CE_BDAA_F50E9EAEF22C */
