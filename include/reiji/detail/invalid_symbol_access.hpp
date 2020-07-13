
// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#pragma once

// REIJI_ON_INVALID_SYMOL is a customization point for library users to handle
// invalid symbol use. Check for its existence, or lack of it, and handle the
// latter case
#if !defined(REIJI_ON_INVALID_SYMBOL)

#    if !defined(REIJI_SYMBOL_THROW_EXCEPTIONS) && !defined(REIJI_SYMBOL_ABORT)
#        if !defined(REIJI_SYMBOL_ABORT)                                       \
            && (defined(__EXCEPTIONS) || defined(_CPPUNWIND)                   \
                || __cpp_exceptions)
#            define REIJI_SYMBOL_THROW_EXCEPTIONS
#        else
#            define REIJI_SYMBOL_ABORT
#        endif
#    elif !defined(REIJI_SYMBOL_ABORT)
#        define REIJI_SYMBOL_ABORT
#    endif

#    ifdef REIJI_SYMBOL_THROW_EXCEPTIONS
// This one assumes we're used only where reiji::bad_symbol_access is defined!
#        define REIJI_ON_INVALID_SYMBOL(x)                                     \
            do {                                                               \
                throw reiji::bad_symbol_access {                               \
                    "reiji: error in " x " (called on invalid symbol"          \
                    ", that is a symbol which is either default constructed "  \
                    "or outlived its origin)"};                                \
            } while (0)
#    elif defined(REIJI_SYMBOL_ABORT)
#        include <cstdio>    // std::abort
#        include <cstdlib>   // std::fputs
#        define REIJI_ON_INVALID_SYMBOL(x)                                     \
            do {                                                               \
                std::fputs("reiji: error in " x " (called on invalid symbol"   \
                           ", that is a symbol which is either default "       \
                           "constructed or outlived its origin)",              \
                           stderr);                                            \
                std::abort();                                                  \
            } while (0)
#    endif

#endif
