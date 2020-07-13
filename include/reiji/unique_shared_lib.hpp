// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_SHARED_LIB_HPP_INCLUDED_
#define UNIQUE_SHARED_LIB_HPP_INCLUDED_

#if defined(_WIN32)
//  We should find a way to minimize the amount of stuff we include in this from
//  this header
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif

#include <cstddef>   // std::nullptr_t
#include <cstdint>   // std::uint64_t
#include <string>
#include <vector>

#include <reiji/symbol.hpp>

namespace reiji {

class unique_shared_lib {
public:
#if defined(_WIN32)
    using flags_type = unsigned long;   // aka what DWORD is typedef'd to
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    using flags_type    = int;   // what is used on *nix's iirc
#endif

    unique_shared_lib() = default;

    unique_shared_lib(const unique_shared_lib&) = delete;
    unique_shared_lib(unique_shared_lib&&) noexcept;

    explicit unique_shared_lib(const char* filename) { open(filename); }
    explicit unique_shared_lib(const std::string& filename) {
        open(filename.c_str());
    }

    unique_shared_lib(const char* filename, flags_type flags) {
        open(filename, flags);
    }
    unique_shared_lib(const std::string& filename, flags_type flags) {
        open(filename.c_str(), flags);
    }

    unique_shared_lib& operator=(const unique_shared_lib&) = delete;
    unique_shared_lib& operator=(unique_shared_lib&&) noexcept;

    ~unique_shared_lib() noexcept;

    void open(const char* filename);
    void open(const char* filename, flags_type flags);

    void open(const std::string& filename) { open(filename.c_str()); }
    void open(const std::string& filename, flags_type flags) {
        open(filename.c_str());
    }

    void close();

    void swap(unique_shared_lib& other);

    template <typename T>
    [[nodiscard]] inline reiji::symbol<T> symbol(const char* sym_name) noexcept;
    template <typename T>
    [[nodiscard]] inline reiji::symbol<T>
    symbol(const std::string& sym_name) noexcept;

    [[nodiscard]] std::string error() const;

private:
    friend class detail::symbol_base;

#if defined(_WIN32)
    using native_handle = ::HMODULE;
    using native_symbol = ::FARPROC;
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    using native_handle = void*;
    using native_symbol = void*;
#endif

    [[nodiscard]] native_symbol _symbol(const char* sym_name);
    std::uint64_t _next_uid() noexcept { return ++_curr_uid; }

    native_handle _handle {nullptr};
    std::uint64_t _curr_uid {0};
    std::string _error;
    std::vector<detail::symbol_base*> _symbols;
};

template <typename T>
inline symbol<T> unique_shared_lib::symbol(const char* sym_name) noexcept {
    return {reinterpret_cast<T*>(_symbol(sym_name)), _next_uid(), this};
}

template <typename T>
inline symbol<T>
unique_shared_lib::symbol(const std::string& sym_name) noexcept {
    return symbol<T>(sym_name.c_str());
}

inline void swap(unique_shared_lib& lhs, unique_shared_lib& rhs) noexcept {
    lhs.swap(rhs);
}

}   // namespace reiji

#undef REIJI_ON_INVALID_SYMBOL

#endif   // UNIQUE_SHARED_LIB_HPP_INCLUDED_
