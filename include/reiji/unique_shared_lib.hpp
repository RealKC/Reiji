// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#ifndef UNIQUE_SHARED_LIB_HPP_INCLUDED_
#define UNIQUE_SHARED_LIB_HPP_INCLUDED_

#include <cstddef>   // std::nullptr_t
#include <cstdint>   // std::uint64_t
#include <filesystem>
#include <string>
#include <vector>

#include <reiji/detail/push_platform_detection_macros.hpp>
#include <reiji/flags.hpp>
#include <reiji/symbol.hpp>

namespace reiji {

namespace fs = std::filesystem;

class unique_shared_lib {
public:
    unique_shared_lib() = default;

    unique_shared_lib(const unique_shared_lib&) = delete;
    unique_shared_lib(unique_shared_lib&&) noexcept;

    explicit unique_shared_lib(const char* filename) { open(filename); }
    explicit unique_shared_lib(const std::string& filename) {
        open(filename.c_str());
    }
    explicit unique_shared_lib(const fs::path& path) { open(path); }

    unique_shared_lib(const char* filename, flags_type flags) {
        open(filename, flags);
    }
    unique_shared_lib(const std::string& filename, flags_type flags) {
        open(filename.c_str(), flags);
    }
    unique_shared_lib(const fs::path& path, flags_type flags) {
        open(path, flags);
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

    void open(const fs::path&);
    void unique_shared_lib::open(const fs::path& path, flags_type flags);

    void close();

    void swap(unique_shared_lib& other);

    template <typename T>
    [[nodiscard]] symbol<T> get_symbol(const char* symbol_name) {
        return symbol<T> {reinterpret_cast<T*>(_get_symbol(symbol_name)),
                          _next_uid(), this};
    }
    template <typename T>
    [[nodiscard]] symbol<T> get_symbol(const std::string& symbol_name) {
        return get_symbol<T>(symbol_name.c_str());
    }

    [[nodiscard]] std::string last_error() const { return _error; }

private:
    friend class detail::symbol_base;

    // It *should* be fine for these to be void* on all the platforms we
    // support, I think.
    using native_handle = void*;
    using native_symbol = void*;

    [[nodiscard]] native_symbol _get_symbol(const char* symbol_name);
    std::uint64_t _next_uid() noexcept { return ++_curr_uid; }

    native_handle _handle {nullptr};
    std::uint64_t _curr_uid {0};
    std::string _error;
    std::vector<detail::symbol_base*> _symbols;
};

inline void swap(unique_shared_lib& lhs, unique_shared_lib& rhs) noexcept {
    lhs.swap(rhs);
}

}   // namespace reiji

#include <reiji/detail/pop_platform_detection_macros.hpp>

#endif   // UNIQUE_SHARED_LIB_HPP_INCLUDED_
