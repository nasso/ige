#ifndef A8C539C5_24BA_48A0_A67B_22700412822C
#define A8C539C5_24BA_48A0_A67B_22700412822C

#include "ige/utility/Types.hpp"
#include <cstdlib>
#include <iostream>
#include <string_view>

namespace ige::utility {

template <class T = void>
[[noreturn]] static T unreachable()
{
    std::abort();
}

template <class T = void>
static T todo(std::string_view file, u64 line)
{
    std::cerr << file << ":" << line << ": todo reached" << std::endl;
    return unreachable<T>();
}

}

#define IGE_TODO(...) ige::utility::todo<__VA_ARGS__>(__FILE__, __LINE__)

#endif /* A8C539C5_24BA_48A0_A67B_22700412822C */
