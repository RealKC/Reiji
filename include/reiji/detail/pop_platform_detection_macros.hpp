// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Lack of include guards intentional

#if defined(REIJI_PLATFORM_WINDOWS) && (defined(_MSC_VER) && !defined(__GCC__))
#    ifdef not
#        undef not
#    endif
#endif

#undef REIJI_PLATFORM_WINDOWS
#undef REIJI_PLATFORM_POSIX
