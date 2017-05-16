#ifndef NOX_UTIL_ASSERT_H_
#define NOX_UTIL_ASSERT_H_
#include <algorithm>
#include <cstdarg>
#include <exception>

#define NOX_ASSERTIONS_ENABLED

template<std::size_t sizeOfFileName>
void
logAssert(const char* expr,
          const char(&filename)[sizeOfFileName],
          const char* function,
          int line, 
          const char* fmt,
          ...)
{
    const char* format = "[ASSERTION FAILED] Expression: %s\nFile: %s\nFunction: %s\nLine: %i\nMessage: ";
    std::printf(format, expr, filename, function, line);

    std::va_list args;
    va_start(args, fmt);
    std::vprintf(fmt, args);
    va_end(args);

    std::printf("\n");
    std::fflush(stdout);
}

/**
 * @brief Custom assert function supporting printf style output.
 *
 * @see Based on Jason Gregory: Game Engine Architecture. p.150.
 */
#ifdef NOX_ASSERTIONS_ENABLED
#define NOX_ASSERT(expr, fmt, ...) \
if (expr) { } \
    else \
    { \
        logAssert<sizeof(__FILE__)>(#expr, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__); \
        std::terminate(); \
    }
#else
#define NOX_ASSERT(expr, fmt, ...)
#endif

#endif
