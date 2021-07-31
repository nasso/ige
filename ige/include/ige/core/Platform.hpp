#ifndef FFC8B3DE_EA2F_4AE8_8911_6993DB10A089
#define FFC8B3DE_EA2F_4AE8_8911_6993DB10A089

#define IGE_STRINGIFY_HELPER(x) #x
#define IGE_STRINGIFY(x) IGE_STRINGIFY_HELPER(x)

#include <version>

#ifndef __INTELLISENSE__

    // Compiler feature testing
    #if __cplusplus < 201703L && (!defined(_MSVC_LANG) || _MSVC_LANG < 201703L)
        #error "Compiler does not support C++17"
    #endif

    // Standard library feature testing
    #if __cpp_lib_concepts < 201907L
        #error "Standard library does not support <concepts>"
    #endif

    #if __cpp_lib_span < 202002L
        #error "Standard library does not support std::span"
    #endif

    #if __cpp_lib_generic_unordered_lookup < 201811L
        #error "Standard library does not support generic unordered lookup"
    #endif

#endif

// platform detection
#if defined(_WIN32)
    #define IGE_PLATFORM_WINDOWS

    #define IGE_EXPORT __declspec(dllexport)
    #define IGE_IMPORT __declspec(dllimport)
#elif defined(__linux__) || defined(__unix__)
    #define IGE_PLATFORM_LINUX

    #define IGE_EXPORT __attribute__((visibility("default")))
    #define IGE_IMPORT __attribute__((visibility("default")))
#else
    #error "Unsupported platform"
#endif

// define IGE_API
#ifdef IGE_BUILD_DLL
    #define IGE_API IGE_EXPORT
#else
    #define IGE_API IGE_IMPORT
#endif

#endif /* FFC8B3DE_EA2F_4AE8_8911_6993DB10A089 */
