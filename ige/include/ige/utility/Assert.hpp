#ifndef A60CA63D_21EB_400D_AEA2_66C6E92B4DD7
#define A60CA63D_21EB_400D_AEA2_66C6E92B4DD7

#if defined(IGE_ENABLE_ASSERT) || defined(IGE_DEBUG)
    #include "ige/utility/Stringify.hpp"
    #include <cstdlib>
    #include <iostream>

    // TODO: use logging library instead of iostream

    #define IGE_ASSERT(x, msg)                                                 \
        if (!(x)) {                                                            \
            std::cerr << __FILE__ << "(" << __LINE__ << ")"                    \
                      << ": (" << IGE_STRING(x)                                \
                      << ") assertion failed: " << msg << std::endl;           \
            std::abort();                                                      \
        }
#else
    #define IGE_ASSERT(x, msg)
#endif

#endif /* A60CA63D_21EB_400D_AEA2_66C6E92B4DD7 */
