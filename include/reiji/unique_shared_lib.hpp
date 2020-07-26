// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_SHARED_LIB_HPP_INCLUDED_
#define UNIQUE_SHARED_LIB_HPP_INCLUDED_

#include <cstddef>   // std::nullptr_t
#include <cstdint>   // std::uint64_t
#include <string>
#include <vector>

#include <reiji/detail/push_platform_detection_macros.hpp>
#include <reiji/symbol.hpp>

#if REIJI_PLATFORM_WINDOWS
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>
#endif

namespace reiji {

/**
  \brief A class representing a shared library
 */
class unique_shared_lib {
public:
#if REIJI_PLATFORM_WINDOWS
    using flags_type = ::DWORD;
#elif REIJI_PLATFORM_POSIX
    using flags_type    = int;   // what is used on *nix's iirc
#endif

    unique_shared_lib() = default;

    unique_shared_lib(const unique_shared_lib&) = delete;
    unique_shared_lib(unique_shared_lib&&) noexcept;
    /**
      \brief Opens a shared library on construction, same semantics as \ref
      unique_shared_lib::open(const char*)
     */
    explicit unique_shared_lib(const char* filename) { open(filename); }
    /**
      \brief Opens a shared library on construction, same semantics as \ref
      unique_shared_lib::open(const char*)
     */
    explicit unique_shared_lib(const std::string& filename) {
        open(filename.c_str());
    }
    /**
      \brief Opens a shared library, passing the \a flags to the low level
      APIs
     */
    unique_shared_lib(const char* filename, flags_type flags) {
        open(filename, flags);
    }
    /**
      \brief Opens a shared library, passing the \a flags to the low level
      APIs
     */
    unique_shared_lib(const std::string& filename, flags_type flags) {
        open(filename.c_str(), flags);
    }

    unique_shared_lib& operator=(const unique_shared_lib&) = delete;
    unique_shared_lib& operator=(unique_shared_lib&&) noexcept;

    ~unique_shared_lib() noexcept;

    /**
      \brief Opens a shared library

      Behaves like LoadLibrary on Windows, and like
      dlopen(\a filename, RTLD_LAZY | RTLD_GLOBAL) on POSIX platforms
     */
    void open(const char* filename);
    /**
      \brief Opens a shared library, passing the \a flags to the low level
      APIs
     */
    void open(const char* filename, flags_type flags);
    /**
      \brief Opens a shared library, same semantics as
      \ref unique_shared_lib::open(const char*)
     */
    void open(const std::string& filename) { open(filename.c_str()); }
    /**
      \brief Opens a shared library, same semantics as
      \ref unique_shared_lib::open(const char*, flags_type)
     */
    void open(const std::string& filename, flags_type flags) {
        open(filename.c_str(), flags);
    }
    /**
      \brief Closes the shared library
     */
    void close();
    /**
      \brief Swaps two unique_shared_lib's
    */
    void swap(unique_shared_lib& other);
    /**
      \brief Extracts a symbol from the shared library, will fail if no
      library is open \tparam T The type of the symbol as it is found in the
      shared library, i.e. for a global variable \rst ``int foo;`` ``T``
      will be ``int``, and for a function ``int bar(int, short)``  it will be
      ``int(int, short)`` \endrst \returns An invalid symbol if the symbol
      isn't found, or no library is open, or the symbol from the library.
     */
    template <typename T>
    [[nodiscard]] symbol<T> get_symbol(const char* symbol_name) {
        return symbol<T> {reinterpret_cast<T*>(_get_symbol(symbol_name)),
                          _next_uid(), this};
    }
    /**
      \brief Extracts a symbol from the shared library, will fail if no
      library is open

      Same semantics as \ref unique_shared_lib::get_symbol(const char*)
     */
    template <typename T>
    [[nodiscard]] symbol<T> get_symbol(const std::string& symbol_name) {
        return get_symbol<T>(symbol_name.c_str());
    }
    /**
      \brief Returns the last error, or an empty string if no error occured.

      \rst
      .. note::

         The last error will be reset if an operation completes
         successfully.
       \endrst
     */
    [[nodiscard]] std::string last_error() const { return _error; }

private:
    friend class detail::symbol_base;

#if REIJI_PLATFORM_WINDOWS
    using native_handle = ::HMODULE;
    using native_symbol = ::FARPROC;
#elif REIJI_PLATFORM_POSIX
    using native_handle = void*;
    using native_symbol = void*;
#endif

    [[nodiscard]] native_symbol _get_symbol(const char* symbol_name);
    std::uint64_t _next_uid() noexcept { return ++_curr_uid; }

    native_handle _handle {nullptr};
    std::uint64_t _curr_uid {0};
    std::string _error;
    std::vector<detail::symbol_base*> _symbols;
};

/**
  \brief Swaps two unique_shared_lib's
 */
inline void swap(unique_shared_lib& lhs, unique_shared_lib& rhs) noexcept {
    lhs.swap(rhs);
}

}   // namespace reiji

#include <reiji/detail/pop_platform_detection_macros.hpp>

#endif   // UNIQUE_SHARED_LIB_HPP_INCLUDED_
