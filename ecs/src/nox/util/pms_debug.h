#ifndef NOX_ECS_PMSDEBUG_H_
#define NOX_ECS_PMSDEBUG_H_
#include <algorithm>
#include <cstdarg>
#include <exception>
#include <mutex>

extern std::mutex g_mutex;

template<std::size_t sizeOfFileName>
void
logDebug(const char(&filename)[sizeOfFileName],
         const char* function,
         int line, 
         const char* fmt,
         ...)
{
    std::lock_guard<std::mutex> lock(g_mutex);
    const char* format = "[DEBUG] Function: %s Line: %i: Message: ";

    std::printf(format, function, line);
    std::va_list args;
    va_start(args, fmt);
    std::vprintf(fmt, args);
    va_end(args);

    std::fflush(stdout);
}

#define PMS_DEBUG(fmt, ...) \
logDebug<sizeof(__FILE__)>(__FILE__,__func__,__LINE__,fmt,##__VA_ARGS__);\
fflush(stdout);

#endif
