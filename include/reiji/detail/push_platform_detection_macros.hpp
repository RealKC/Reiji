// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Lack of include guards intentional

#if defined(_WIN32)
#    define REIJI_PLATFORM_WINDOWS 1
#    define REIJI_PLATFORM_POSIX   0
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)            \
    || defined(__HAIKU__)
#    define REIJI_PLATFORM_WINDOWS 0
#    define REIJI_PLATFORM_POSIX   1
#else
#    error "Unsupported platform. (Maybe send a PR?)"
#endif
