
// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_SHARED_LIB_HPP_INCLUDED_
#define UNIQUE_SHARED_LIB_HPP_INCLUDED_

#if defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include <string>
#include <type_traits>

namespace reiji {
    class unique_shared_lib {

    public:
        unique_shared_lib();
        unique_shared_lib(const unique_shared_lib&) = delete;
        unique_shared_lib(unique_shared_lib&&) noexcept;
        inline explicit unique_shared_lib(const char* filename) {
            open(filename);
        }
        inline explicit unique_shared_lib(const std::string& filename) {
            open(filename.c_str());
        }

        unique_shared_lib& operator=(const unique_shared_lib&) = delete;
        unique_shared_lib& operator=(unique_shared_lib&&) noexcept;

        ~unique_shared_lib() noexcept;

        void open(const char* filename);
        inline void open(const std::string& filename) {
            open(filename.c_str());
        }
        void close();

        template<typename T>
        [[nodiscard]] inline T symbol(const char* sym_name);
        template<typename T>
        [[nodiscard]] inline T symbol(const std::string& sym_name);

        [[nodiscard]] std::string error() const;
    private:
#if defined(_WIN32)
        using handle_t = ::HMODULE;
        using symbol_t = ::FARPROC;
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
        using handle_t = void*;
        using symbol_t = void*;
#endif
        [[nodiscard]] symbol_t _symbol(const char* sym_name);

        handle_t _handle;
        std::string _error;
    };

    template<typename T>
    inline T unique_shared_lib::symbol(const char* sym_name) {
        static_asssert(std::is_pointer<T>::value,
            "The type parameter for unique_shared_lib::symbol_as must be a pointer");
        return reinterpret_cast<T>(_symbol(sym_name));
    }

    template<typename T>
    inline T unique_shared_lib::symbol(const std::string& sym_name) {
        return symbol<T>(sym_name.c_str());
    }
} // reiji

#endif // UNIQUE_SHARED_LIB_HPP_INCLUDED_
