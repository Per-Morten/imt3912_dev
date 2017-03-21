#ifndef NOX_MEMORY_BYTE_H_
#define NOX_MEMORY_BYTE_H_

namespace nox
{
    namespace memory
    {
        /**
         * @brief Typealias used to indicate a byte.
         *        Should probably be changed to std::byte in C++17.
         *        http://en.cppreference.com/w/cpp/types/byte
         */ 
        using Byte = unsigned char;
    }
}

#endif
