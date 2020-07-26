
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

/**
  \file
*/
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
        // NOTE: The lack of protection against self assignment is intentional
        // as this only gets called in places where we know that to be the case.
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
        return is_valid() && _origin == other._origin;
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

/**
  \brief An except that may be thrown when invalid symbols are used.

  \rst
  .. note::

     This exception type is only ever thrown if \ref REIJI_ON_INVALID_SYMBOL
     is configugred to throw it(which, if exceptions are enabled, by default
     it is).
  \endrst
 */
class bad_symbol_access : public std::runtime_error {
public:
    explicit bad_symbol_access(const char* s) : runtime_error {s} {}
    virtual ~bad_symbol_access() noexcept = default;
};

/**
  \brief A class representing a non-function symbol extracted from a shared
  library.
 */
template <typename T>
class symbol final : private detail::symbol_base {
public:
    using element_type    = T;
    using pointer         = element_type*;
    using const_pointer   = const element_type*;
    using reference       = element_type&;
    using const_reference = const element_type&;

    /**
      \brief Default constructs a symbol in an invalid state.
     */
    symbol() noexcept = default;

    symbol(const symbol&) = delete;
    symbol& operator=(const symbol&) = delete;

    /**
      \brief Move constructs a symbol from another one.

      The other symbol is left in invalid state.
     */
    symbol(symbol&& other) noexcept { *this = std::move(other); }

    /**
      \brief Move assigns from another symbol.

      The other symbol is left in an invalid state.
     */
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

    /**
      \brief Gives you a mutable reference to the object inside the shared
      library.

      Will call \ref REIJI_ON_INVALID_SYMBOL if the symbol it is called on is
      not valid.
     */
    reference operator*() {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator*");
        }
    }

    /**
      \brief Gives you an immutable reference to the object inside the
      shared library.

      Will call \ref REIJI_ON_INVALID_SYMBOL if the symbol it is called on
      is not valid.
     */
    const_reference operator*() const {
        if (is_valid()) {
            return *_ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator* const");
        }
    }

    /**
      \brief

      Will call \ref REIJI_ON_INVALID_SYMBOL if the symbol it is called on is
      not valid.
     */
    pointer operator->() {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator->");
        }
    }

    /**
     \brief

      Will call \ref REIJI_ON_INVALID_SYMBOL if the symbol it is called on
      is not valid.
     */
    const_pointer operator->() const {
        if (is_valid()) {
            return _ptr;
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<T>::operator-> const");
        }
    }

    /**
      \brief Swaps the two symbols.
     */
    void swap(symbol& other) noexcept {
        symbol_base::swap(other);
        std::swap(_ptr, other._ptr);
    }

    /**
      \brief Tells you whether a symbol is valid or not.
     */
    bool is_valid() const noexcept { return symbol_base::is_valid() && _ptr; }

    /**
      \brief Tells you whether two symbols share their origin library or not.

      \rst
      .. note::

         Invalid symbols do not share origin with any other symbol, including
         other invalid symbols.
      \endrst
     */
    template <typename U>
    bool shares_origin_with(const symbol<U>& other) const noexcept {
        return symbol_base::shares_origin_with(other);
    }

    /**
      \brief Behaviour identical to \ref is_valid()
     */
    explicit operator bool() const noexcept { return is_valid(); }

    /**
      \brief Negation of \ref is_valid()
    */
    bool operator!() const noexcept { return not is_valid(); }

    /**
      \brief Checks if the symbol is in a null-like state.

      A symbol may be in this state if it invalid, or if the low level APIs
      wrapped by Reiji did not find a symbol for the name you provided.
     */
    bool operator==(std::nullptr_t) const noexcept { return not _ptr; }

    /**
      \brief Compares two symbols for equality.

      \rst
      .. note::
         * If either symbol is invalid, this will return false.
         * If the symbols do not share origin, this will return false.
      \endrst
     */
    bool operator==(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 0;
    }

    /**
      \brief Compares two symbols for a 'less than' relationship.

      \rst
      .. note::

         * If either symbol is invalid, this will return false.
         * If the symbols do not share origin, this will return false.
      \endrst
    */
    bool operator<(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == -1;
    }

    /**
      \brief Compares two symbols for a 'greater-than' relationship

      \rst
      .. note::

         * If either symbol is invalid, this will return false.
         * If the symbols do not share origin, this will return false.
      \endrst
     */
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

/**
  \brief A class representing a function symbol extracted from a shared
  library.
 */
template <typename R, typename... Args>
class symbol<R(Args...)> final : private detail::symbol_base {
public:
    using element_type    = R(Args...);
    using pointer         = element_type*;
    using const_pointer   = const element_type*;
    using reference       = element_type&;
    using const_reference = const element_type&;

    /**
      \brief Default constructs a symbol in an invalid state.
     */
    symbol() noexcept = default;

    symbol(const symbol&) = delete;
    symbol& operator=(const symbol&) = delete;

    /**
      \brief Move constructs a symbol from another one.

      The other symbol is left in invalid state.
     */
    symbol(symbol&& other) noexcept { *this = std::move(other); }

    /**
      \brief Move assigns from another symbol.

      The other symbol is left in an invalid state.
     */
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

    /**
      \brief Calls the function inside the shared library.
      \param args... The parameters to be passed to the function inside the
      shared library
      \returns The return value of the function inside the library.
     */
    R operator()(Args... args) {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<R(Args...)>::operator()");
        }
    }

    /**
      \brief Calls the function inside the shared library.
      \param args... The parameters to be passed to the function inside the
      shared library
      \returns The return value of the function inside the library.
     */
    R operator()(Args... args) const {
        if (is_valid()) {
            return (*_f)(args...);
        } else {
            // clang-format off
            REIJI_ON_INVALID_SYMBOL("reiji::symbol<R(Args...)>::operator() const");
            // clang-format on
        }
    }

    /**
      \brief Swaps the two symbols.
     */
    void swap(symbol& other) noexcept {
        symbol_base::swap(other);
        std::swap(_f, other._f);
    }

    /**
      \brief Tells you whether a symbol is valid or not.
     */
    bool is_valid() const noexcept { return symbol_base::is_valid() && _f; }

    /**
      \brief Tells you whether two symbols share their origin library or not.

      \rst
      .. note::

         Invalid symbols do not share origin with any other symbol, including
         other invalid symbols.
      \endrst
     */
    template <typename U>
    bool shares_origin_with(const symbol<U>& other) const noexcept {
        return symbol_base::shares_origin_with(other);
    }

    /**
    \brief Behaviour identical to \ref is_valid() explicit
    */
    operator bool() const noexcept { return is_valid(); }

    /**
      \brief Negation of \ref is_valid()
     */
    bool operator!() const noexcept { return not is_valid(); }

    /**
      \brief Checks if the symbol is in a null-like state.

      A symbol may be in this state if it invalid, or if the low level APIs
      wrapped by Reiji did not find a symbol for the name you provided.
     */
    bool operator==(std::nullptr_t) const noexcept { return not _f; }

    /**
      \brief Compares two symbols for equality.

      \rst
      .. note::

         * Two invalid symbols will not compare equal.
         * If the symbols do not share origin, this will return false.
      \endrst
     */
    bool operator==(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 0;
    }
    /**
      \brief Compares two symbols for a 'less than' relationship.

      \rst
      .. note::

         * If either symbol is invalid, this will return false.
         * If the symbols do not share origin, this will return false.
      \endrst
    */
    bool operator<(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == -1;
    }
    /**
      \brief Compares two symbols for a 'greater-than' relationship

      \rst
      .. note::

         *If either symbol is invalid, this will return false.
         * If the symbols do not share origin, this will return false.
      \endrst
     */
    bool operator>(const symbol& rhs) const noexcept {
        return symbol_base::shares_origin_with(rhs)
               && symbol_base::compare(rhs) == 1;
    }

private:
    friend class unique_shared_lib;

    symbol(pointer f, std::uint64_t uid, unique_shared_lib* origin)
        : _f {f}, symbol_base {uid, origin} {}

    pointer _f {nullptr};
};

#ifdef __DOXYGEN__

/**
  \brief Checks if the symbol is in a null-like state.

  A symbol may be in this state if it invalid, or if the low level APIs
  wrapped by Reiji did not find a symbol for the name you provided.
 */
template <typename T>
bool operator==(std::nullptr_t, const symbol<T>& rhs) noexcept;

/**
  \brief Checks two symbols for inequality.

  \rst
  .. note::

     This function will return false for two invalid symbols.
  \endrst
 */
template <typename T>
bool operator!=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept;

/**
  \brief Checks if the two symbols have a 'less-than or equal' relationship.

  \rst
  .. note::

     * If either symbol is invalid, this will return false.
     * If the symbols do not share origin, this will return false.
  \endrst
 */
template <typename T>
bool operator<=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept;

/**
  \brief Checks if the two symbols have a 'greather-than or equal'
  relationship.
  \rst
  .. note::

     * If either symbol is invalid, this will return false.
     * If the symbols do not share origin, this will return false.
  \endrst
 */
template <typename T>
bool operator>=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept;

#else

template <typename T>
bool operator==(std::nullptr_t, const symbol<T>& rhs) noexcept {
    return rhs == nullptr;
}

template <typename T>
bool operator!=(const symbol<T>& lhs, std::nullptr_t) noexcept {
    return not(lhs == nullptr);
}

template <typename T>
bool operator!=(std::nullptr_t, const symbol<T>& rhs) noexcept {
    return rhs != nullptr;
}

template <typename T>
bool operator!=(const symbol<T>& lhs, const symbol<T>& rhs) noexcept {
    return not lhs.shares_origin_with(rhs) && not(lhs == rhs);
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

#endif

}   // namespace reiji
