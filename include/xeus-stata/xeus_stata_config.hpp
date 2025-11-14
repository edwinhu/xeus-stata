#ifndef XEUS_STATA_CONFIG_HPP
#define XEUS_STATA_CONFIG_HPP

#define XEUS_STATA_VERSION_MAJOR 0
#define XEUS_STATA_VERSION_MINOR 1
#define XEUS_STATA_VERSION_PATCH 0

#define XEUS_STATA_VERSION "0.1.0"

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define XEUS_STATA_PLATFORM_WINDOWS
#elif defined(__APPLE__) && defined(__MACH__)
    #define XEUS_STATA_PLATFORM_MACOS
#elif defined(__linux__)
    #define XEUS_STATA_PLATFORM_LINUX
#endif

// Default Stata path (can be overridden via environment variable)
#ifndef DEFAULT_STATA_PATH
    #if defined(XEUS_STATA_PLATFORM_WINDOWS)
        #define DEFAULT_STATA_PATH "C:\\Program Files\\Stata18\\StataMP-64.exe"
    #elif defined(XEUS_STATA_PLATFORM_MACOS)
        #define DEFAULT_STATA_PATH "/Applications/Stata/stata"
    #else
        #define DEFAULT_STATA_PATH "/usr/local/stata/stata"
    #endif
#endif

#endif // XEUS_STATA_CONFIG_HPP
