#ifndef NOX_ECS_PMSDEBUG_H_
#define NOX_ECS_PMSDEBUG_H_
#include <algorithm>
#include <cstdarg>
#include <exception>
#include <mutex>
#include <chrono>

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
    //const auto now = std::chrono::high_resolution_clock::now();
    using Ms = std::chrono::milliseconds;
    double timeInMs =
        std::chrono::duration<double, Ms::period>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();

    //const auto duration = std::chrono::duration<float, std::chrono::seconds::period>(now).count();
    const char* format = "%f [DEBUG] Function: %s Line: %-5i: Message: ";

    std::printf(format, timeInMs, function, line);
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
