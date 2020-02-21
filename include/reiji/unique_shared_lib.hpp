
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

// Customization points for symbol beaviour
#ifdef REIJI_SYMBOL_THROW_EXCEPTIONS
#   define REIJI_ON_INVALID_SYMBOL(x) \
        do { \
            throw reiji::bad_symbol_access { \
                "reiji: error in " #x " (called on invalid symbol" \
                ", that is a symbol which is either default constructed or " \
                "outlived its origin)" \
            }; \
        } while(0)
#elif defined REIJI_SYMBOL_ABORT
#   include <cstdlib>
#   include <cstdio>
#   define REIJI_ON_INVALID_SYMBOL(x) \
        do { \
            std::fputs( \
                "reiji: error in " #x " (called on invalid symbol" \
                ", that is a symbol which is either default constructed or " \
                "outlived its origin)", stderr); \
            std::abort(); \
        } while(0)
// if neither macro is defined, we simply check if exceptions are enabled and choose
// one of the previous models
#elif defined(__EXCEPTIONS) || defined(_CPPUNWIND) || __cpp_exceptions
#   define REIJI_ON_INVALID_SYMBOL(x) \
        do { \
            throw reiji::bad_symbol_access { \
                "reiji: error in " #x " (called on invalid symbol" \
                ", that is a symbol which is either default constructed or " \
                "outlived its origin)" \
            }; \
        } while(0)
#else
#   include <cstdlib>
#   include <cstdio>
#   define REIJI_ON_INVALID_SYMBOL(x) \
        do { \
            std::fputs( \
                "reiji: error in " #x " (called on invalid symbol" \
                ", that is a symbol which is either default constructed or " \
                "outlived its origin)", stderr); \
            std::abort(); \
        } while(0)
#endif

#include <cstdint>
#include <string>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace reiji {

class unique_shared_lib;

namespace detail {
class sym {
protected:
    sym() noexcept = default;
    sym(std::uint64_t uid, reiji::unique_shared_lib* origin)
        noexcept : _uid{uid}, _origin{origin} {}

    void move_into_self_from(sym& other) noexcept {
        _uid = other._uid;
        other._uid = 0;
        _origin = other._origin;
        other._origin = nullptr;
    }

    void remove_self() noexcept;

    bool is_valid() const noexcept {
        return _uid && _origin;
    }

    int compare(const sym& other) const noexcept {
        if (_uid < other._uid) {
            return -1;
        } else if (_uid > other._uid) {
            return 1;
        } else {
            return 0;
        }
    }

    bool share_origin(const sym& other) const noexcept {
        return _origin == other._origin;
    }
private:
    friend class reiji::unique_shared_lib;

    void invalidate() noexcept {
        _uid = 0;
        _origin = nullptr;
    }

    std::uint64_t _uid {0};
    reiji::unique_shared_lib* _origin {nullptr};
};

} // detail

class bad_symbol_access : std::runtime_error {
public:
    bad_symbol_access(const char* s)
        : runtime_error {s} {}
    virtual ~bad_symbol_access() noexcept = default;
};

template <typename T>
class symbol final : private detail::sym {
public:
    using element_type = T;
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;

    symbol() noexcept = default;
    symbol(const symbol&) = delete;
    symbol(symbol&& other) noexcept {
        *this = std::move(other);
    }

    symbol& operator=(const symbol&) = delete;
    symbol& operator=(symbol&& other) noexcept {
        if (this != &other) {
            move_into_self_from(other);
            _ptr = other._ptr;
            other._ptr = nullptr;
        }
    }

    ~symbol() noexcept {
        remove_self();
    }

    reference operator*() {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator*);
        }
    }

    const_reference operator*() const {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator* const);
        }
    }

    pointer operator->() {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator->);
        }
    }

    const_pointer operator->() const {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator-> const);
        }
    }

    reference operator[](std::size_t idx) {
        if (is_valid()) {
            return _ptr[idx];
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator[]);
        }
    }

    const_reference operator[](std::size_t idx) const {
        if (is_valid()) {
            return _ptr[idx];
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<T>::operator[] const);
        }
    }

    bool is_valid() const noexcept {
        return sym::is_valid() && _ptr;
    }

    template <typename U>
    bool share_origin(const symbol<U>& other) const noexcept {
        return sym::share_origin(other);
    }

    operator bool() const noexcept {
        return is_valid();
    }

    bool operator==(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && sym::compare(rhs) == 0;
    }

    bool operator<(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && sym::compare(rhs) == -1;
    }

    bool operator>(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && sym::compare(rhs) == 1;
    }

private:
    friend class unique_shared_lib;

    symbol(pointer ptr, std::uint64_t uid, unique_shared_lib* origin)
        noexcept : _ptr{ptr}, sym{uid, origin} {}

    pointer _ptr {nullptr};
    };

template <typename R, typename... Args>
class symbol<R(Args...)> final : private detail::sym {
public:
    using element_type = R(Args...);
    using pointer = element_type*;
    using const_pointer = const element_type*;
    using reference = element_type&;
    using const_reference = const element_type&;

    symbol() noexcept = default;
    symbol(const symbol&) = delete;
    sumbol(symbol&& other) noexcept {
        *this = std::move(other);
    }

    symbol& operator=(const symbol&) = delete;
    symbol& operator=(symbol&& other) noexcept {
        if (this != &other) {
            move_into_self_from(other);
            _f = other._f;
            other._f = nullptr;
        }
    }

    ~symbol() noexcept {
        remove_self();
    }

    R operator()(Args... args) {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<R(Args...)>::operator());
        }
    }

    R operator()(Args... args) const {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            REIJI_ON_INVALID_SYMBOL(reiji::symbol<R(Args...)>::operator() const);
        }
    }

    bool is_valid() const noexcept {
        return sym::is_valid() && _f;
    }

    template <typename U>
    bool share_origin(const symbol<U>& other) const noexcept {
        return sym::share_origin(other);
    }

    operator bool() const noexcept {
        return is_valid();
    }


    bool operator==(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && sym::compare(rhs) == 0;
    }

    bool operator<(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && sym::compare(rhs) == -1;
    }

    bool operator>(const symbol& rhs) const noexcept {
        return sym::share_origin(rhs) && compare(rhs) == 1;
    }
private:
    friend class unique_shared_lib;

    symbol(pointer f, std::uint64_t uid, unique_shared_lib* origin)
        noexcept : _f{f}, sym{uid, origin} {}

    pointer _f {nullptr};
};

template <typename T>
bool operator!=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.share_origin(rhs) && !(lhs == rhs)
}

template <typename T>
bool operator<=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.share_origin(rhs) && !(lhs > rhs);
}

template <typename T>
bool operator>=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.share_origin(rhs) && !(lhs < rhs);
}


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
    [[nodiscard]]
    inline reiji::symbol<T> symbol(const char* sym_name) noexcept;
    template<typename T>
    [[nodiscard]]
    inline reiji::symbol<T> symbol(const std::string& sym_name) noexcept;

    [[nodiscard]] std::string error() const;
private:
    friend class detail::sym;

#if defined(_WIN32)
    using native_handle = ::HMODULE;
    using native_symbol = ::FARPROC;
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
    using native_handle = void*;
    using native_symbol = void*;
#endif

    [[nodiscard]] native_symbol _symbol(const char* sym_name);
    std::uint64_t _next_uid() {
        return ++_curr_uid;
    }

    native_handle _handle;
    std::uint64_t _curr_uid {0};
    std::string _error;
    std::vector<detail::sym*> _symbols;
};

template<typename T>
inline symbol<T> unique_shared_lib::symbol(const char* sym_name) noexcept {
    return symbol<T> {
        reinterpret_cast<T>(_symbol(sym_name)),
        _next_uid(),
        this
    };
}

template<typename T>
inline symbol<T> unique_shared_lib::symbol(const std::string& sym_name) noexcept {
    return symbol<T>(sym_name.c_str());
}

} // reiji

#endif // UNIQUE_SHARED_LIB_HPP_INCLUDED_
