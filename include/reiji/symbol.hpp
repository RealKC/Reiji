
// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>     // std::size_t
#include <cstdint>     // std::uint64_t
#include <stdexcept>   // std::runtime_error
#include <utility>     // std::move, std::swap

#include <reiji/detail/invalid_symbol_access.hpp>

namespace reiji {

class unique_shared_lib;

namespace detail {

class symbol_base {
protected:
    symbol_base() noexcept = default;
    symbol_base(std::uint64_t uid, reiji::unique_shared_lib* origin);

    symbol_base(symbol_base&) = delete;
    symbol_base& operator=(symbol_base&) = delete;

    symbol_base(symbol_base&& other) noexcept { *this = std::move(other); }

    symbol_base& operator=(symbol_base&& other) noexcept {
        // Lack of self assignment protection is intentional
        _uid    = std::exchange(other._uid, 0);
        _origin = std::exchange(other._origin, nullptr);
        return *this;
    }

    void remove_self_from_origins_symbol_vector() noexcept;

    bool is_valid() const noexcept { return _uid && _origin; }

    int compare(const symbol_base& other) const noexcept {
        if (_uid < other._uid) {
            return -1;
        } else if (_uid > other._uid) {
            return 1;
        } else {
            return 0;
        }
    }

    void swap(symbol_base& other) noexcept {
        using std::swap;
        swap(_uid, other._uid);
        swap(_origin, other._origin);
    }

    bool shares_origin_with(const symbol_base& other) const noexcept {
        return _origin == other._origin;
    }

private:
    friend class reiji::unique_shared_lib;

    void _invalidate() noexcept {
        _uid    = 0;
        _origin = nullptr;
    }

    std::uint64_t _uid {0};
    reiji::unique_shared_lib* _origin {nullptr};
};

}   // namespace detail

class bad_symbol_access : std::runtime_error {
public:
    bad_symbol_access(const char* s) : runtime_error {s} {}
    virtual ~bad_symbol_access() noexcept = default;
};

template <typename T>
class symbol final : private detail::symbol_base {
public:
    using element_type    = T;
    using pointer         = element_type*;
    using const_pointer   = const element_type*;
    using reference       = element_type&;
    using const_reference = const element_type&;

    symbol() noexcept = default;

    symbol(const symbol&) = delete;
    symbol& operator=(const symbol&) = delete;

    symbol(symbol&& other) noexcept { *this = std::move(other); }

    symbol& operator=(symbol&& other) noexcept {
        if (this != &other) {
            _ptr = std::exchange(other._ptr, nullptr);

            return static_cast<symbol&>(
                symbol_base::operator=(std::move(other)));
        }
    }

    ~symbol() noexcept {
        symbol_base::remove_self_from_origins_symbol_vector();
    }

    reference operator*() {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator*");
        }
    }

    const_reference operator*() const {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator* const");
        }
    }

    pointer operator->() {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator->");
        }
    }

    const_pointer operator->() const {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator-> const");
        }
    }

    void swap(symbol& other) noexcept {
        symbol_base::swap(other);
        std::swap(_ptr, other._ptr);
    }

    bool is_valid() const noexcept { return symbol_base::is_valid() && _ptr; }

    template <typename U>
    bool shares_origin_with(const symbol<U>& other) const noexcept {
        return symbol_base::shares_origin_with(other);
    }

    explicit operator bool() const noexcept { return is_valid(); }

    bool operator!() const noexcept { return not is_valid(); }

    bool operator==(std::nullptr_t) const noexcept { return not _ptr; }

    bool operator==(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 0;
    }

    bool operator<(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == -1;
    }

    bool operator>(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 1;
    }

private:
    friend class unique_shared_lib;

    symbol(pointer ptr, std::uint64_t uid, unique_shared_lib* origin)
        : _ptr {ptr}, symbol_base {uid, origin} {}

    pointer _ptr {nullptr};
};

template <typename R, typename... Args>
class symbol<R(Args...)> final : private detail::symbol_base {
public:
    using element_type    = R(Args...);
    using pointer         = element_type*;
    using const_pointer   = const element_type*;
    using reference       = element_type&;
    using const_reference = const element_type&;

    symbol() noexcept = default;

    symbol(const symbol&) = delete;
    symbol& operator=(const symbol&) = delete;

    symbol(symbol&& other) noexcept { *this = std::move(other); }

    symbol& operator=(symbol&& other) noexcept {
        if (this != &other) {
            _f = std::exchange(other._f, nullptr);

            return static_cast<symbol&>(
                symbol_base::operator=(std::move(other)));
        }
    }

    ~symbol() noexcept {
        symbol_base::remove_self_from_origins_symbol_vector();
    }

    R operator()(Args... args) {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<R(Args...)>::operator()");
        }
    }

    R operator()(Args... args) const {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            // clang-format off
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<R(Args...)>::operator() const");
            // clang-format on
        }
    }

    void swap(symbol& other) noexcept {
        symbol_base::swap(other);
        std::swap(_f, other._f);
    }

    bool is_valid() const noexcept { return symbol_base::is_valid() && _f; }

    template <typename U>
    bool shares_origin_with(const symbol<U>& other) const noexcept {
        return symbol_base::shares_origin_with(other);
    }

    explicit operator bool() const noexcept { return is_valid(); }

    bool operator!() const noexcept { return not is_valid(); }

    bool operator==(std::nullptr_t) const noexcept { return not _f; }

    bool operator==(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 0;
    }

    bool operator<(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == -1;
    }

    bool operator>(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs) && compare(rhs) == 1;
    }

private:
    friend class unique_shared_lib;

    symbol(pointer f, std::uint64_t uid, unique_shared_lib* origin)
        : _f {f}, symbol_base {uid, origin} {}

    pointer _f {nullptr};
};

template <typename T>
bool operator==(std::nullptr_t null, const symbol<T>& rhs) noexcept {
    return rhs == null;
}

template <typename T>
bool operator!=(std::nullptr_t null, const symbol<T>& rhs) noexcept {
    return not(rhs == nullptr);
}

template <typename T>
bool operator!=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.shares_origin_with(rhs) && not(lhs == rhs);
}

template <typename T>
bool operator<=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.shares_origin_with(rhs) && not(lhs > rhs);
}

template <typename T>
bool operator>=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return lhs.shares_origin_with(rhs) && not(lhs < rhs);
}

template <typename T>
void swap(symbol<T>& lhs, symbol<T>& rhs) noexcept {
    lhs.swap(rhs);
}
}   // namespace reiji
