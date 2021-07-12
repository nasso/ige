#include "igepch.hpp"

#include "ige/core/Task.hpp"

using ige::core::BadTaskAccess;

const char* BadTaskAccess::what() const noexcept
{
    return "value() called on an unfulfilled task";
}
