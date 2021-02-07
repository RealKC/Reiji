
// Copyright Mițca Dumitru 2021 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <reiji/detail/push_platform_detection_macros.hpp>

#if REIJI_PLATFORM_POSIX
#    include <dlfcn.h>
#endif

namespace reiji {

namespace detail {

#if REIJI_PLATFORM_WINDOWS
// Definition taken from
// <https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types>
// Webarchive
// <https://web.archive.org/web/20210128143152/https://docs.microsoft.com/en-us/windows/win32/winprog/windows-data-types>
using raw_flags_type = unsigned long;
#elif REIJI_PLATFORM_POSIX
using raw_flags_type = int;   // what is used on *nix's iirc
#endif

}   // namespace detail

enum flags_type : detail::raw_flags_type {};

#define REIJI_RAW(x) (static_cast<::reiji::detail::raw_flags_type>(x))

constexpr flags_type operator|(flags_type lhs, flags_type rhs) noexcept {
    return flags_type {REIJI_RAW(lhs) | REIJI_RAW(rhs)};
}

constexpr flags_type operator&(flags_type lhs, flags_type rhs) noexcept {
    return flags_type {REIJI_RAW(lhs) & REIJI_RAW(rhs)};
}

constexpr flags_type operator^(flags_type lhs, flags_type rhs) noexcept {
    return flags_type {REIJI_RAW(lhs) ^ REIJI_RAW(rhs)};
}

constexpr flags_type operator~(flags_type f) noexcept {
    return flags_type {~REIJI_RAW(f)};
}

constexpr flags_type& operator|=(flags_type& lhs, flags_type rhs) noexcept {
    lhs = lhs | rhs;
    return lhs;
}

constexpr flags_type& operator&=(flags_type& lhs, flags_type rhs) noexcept {
    lhs = lhs & rhs;
    return lhs;
}

constexpr flags_type& operator^=(flags_type& lhs, flags_type rhs) noexcept {
    lhs = lhs ^ rhs;
    return lhs;
}

#undef REIJI_RAW

namespace win {

#if REIJI_PLATFORM_WINDOWS
constexpr static auto load_ignore_code_authz_level = flags_type {0x00000010ul};
constexpr static auto load_library_search_application_dir =
    flags_type {0x00000200ul};
constexpr static auto load_library_search_default_dirs =
    flags_type {0x00001000ul};
constexpr static auto load_library_search_dll_load_dir =
    flags_type {0x00000100ul};
constexpr static auto load_library_search_system32  = flags_type {0x00000800ul};
constexpr static auto load_library_search_user_dirs = flags_type {0x00000400ul};
constexpr static auto load_with_altered_search_path = flags_type {0x00000008ul};
constexpr static auto load_library_require_signed_target =
    flags_type {0x00000080ul};
constexpr static auto load_library_safe_current_dirs =
    flags_type {0x00002000ul} l
#else
constexpr static auto load_ignore_code_authz_level        = flags_type {0ul};
constexpr static auto load_library_search_application_dir = flags_type {0ul};
constexpr static auto load_library_search_default_dirs    = flags_type {0ul};
constexpr static auto load_library_search_dll_load_dir    = flags_type {0ul};
constexpr static auto load_library_search_system32        = flags_type {0ul};
constexpr static auto load_library_search_user_dirs       = flags_type {0ul};
constexpr static auto load_with_altered_search_path       = flags_type {0ul};
constexpr static auto load_library_require_signed_target  = flags_type {0ul};
constexpr static auto load_library_safe_current_dirs      = flags_type {0ul};
#endif
}   // namespace win

namespace posix {

#if REIJI_PLATFORM_POSIX
constexpr static auto rtld_lazy   = flags_type {RTLD_LAZY};
constexpr static auto rtld_now    = flags_type {RTLD_NOW};
constexpr static auto rtld_global = flags_type {RTLD_GLOBAL};
constexpr static auto rltd_local  = flags_type {RTLD_LOCAL};
#else
constexpr static auto rtld_lazy                           = flags_type {0};
constexpr static auto rtld_now                            = flags_type {0};
constexpr static auto rtld_global                         = flags_type {0};
constexpr static auto rltd_local                          = flags_type {0};
#endif
}   // namespace posix

}   // namespace reiji

#include <reiji/detail/pop_platform_detection_macros.hpp>
