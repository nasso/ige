#include "CallOnExit.hpp"
#include <concepts>
#include <functional>

namespace ige::utility {

template <std::invocable<> F>
CallOnExit::CallOnExit(F&& f)
    : m_callback(std::forward<F>(f))
{
}

template <std::invocable<> F>
CallOnExit::~CallOnExit()
{
    m_callback();
}

}
