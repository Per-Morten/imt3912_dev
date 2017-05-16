#ifndef NOX_UTIL_CYCLECOUNT_H_
#define NOX_UTIL_CYCLECOUNT_H_

#include <cstdint>
#ifdef _WIN32
#include <intrin.h>
#endif

namespace nox
{
    namespace util
    {
        /**
         * @brief      Gets the number of cpu cycles from the cpu. Generally it
         *             does a rdtsc command.
         *
         * @return     The cpu cycle count.
         *
         * @note       Taken from stack overflow, minor renames have been done
         *             for clarity, standard adaption as well as a changed
         *             include.
         *
         * @see        http://stackoverflow.com/questions/13772567/get-cpu-cycle-count
         */
        inline
        std::uint64_t getCpuCycleCount()
        {
            #ifdef _WIN32
                return __rdtsc();
            #else
                unsigned int low;
                unsigned int high;
                __asm__ __volatile__ ("rdtsc" : "=a" (low), "=d" (high));
                return (static_cast<std::uint64_t>(high) << 32) | low;
            #endif
        }
    }
}

#endif
