#pragma once
#include <system_error>
#include <memory>
#include <string_view>
#include <string>
#include <cxxabi.h>

namespace utils {

    inline std::string demangle(std::string_view name)
    {
        struct deleter
        {
            void operator()(char* ptr) const noexcept
            {
                free(ptr);
            }
        };
        size_t length = 0;
        int status = -1;
        std::unique_ptr<char, deleter> demangled_name{abi::__cxa_demangle(name.data(), nullptr, &length, &status)};
        switch(status) {
            case 0: return std::string(demangled_name.get(), length);
            case -1: throw std::bad_alloc();
            case -2: throw std::system_error(make_error_code(std::errc::illegal_byte_sequence));
            case -3: throw std::system_error(make_error_code(std::errc::invalid_argument));
            default: throw std::system_error(make_error_code(std::errc::invalid_argument));
        }
    }

}