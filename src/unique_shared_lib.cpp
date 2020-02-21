
// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#include "unique_shared_lib.hpp"

#if defined(_WIN32)
#   define PLATFORM_WINDOWS 1
#   include <cstddef> // std::size_t
#   include <new>
#   include <windows.h>
#elif defined(__linux__) || defined(__APPLE__) || defined(__unix__)
#   define PLATFORM_POSIX 1
#   include <dlfcn.h>
#else
#   error Unsupported platform.
#endif

#include <algorithm>
#include <utility> // std::move

namespace reiji {

namespace detail {

void sym::remove_self() {
    // We can binary search to find ourselves as we always push to the back of
    // the _symbols vector in unique_shared_lib
    std::size_t low = 0, high = _origin->_symbols.size() - 1, mid;
    while (low <= high) {
        mid = (low + high) / 2;
        if (_origin->_symbols[mid]->_uid > _uid) {
            high = mid - 1;
        } else if (_origin->_symbols[mid]->_uid < _uid) {
            low = mid + 1;
        } else {
            break; // found ourselves!
        }
    }
    _origin->_symbols[mid] = nullptr;

}

}

#if defined(PLATFORM_WINDOWS)
static inline std::string get_error(DWORD err_code) {
    ::LPSTR buf = nullptr;
    std::size_t len = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER
            | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        err_code,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (::LPSTR)&buf,
        0,
        nullptr);
    try {
        std::string error(buf, len);
        ::LocalFree(buf);
        return error;
    } catch (...) {
        // In case std::string's constructor throws an exception, we make sure
        // to not leak resources and rethrow the exception.
        ::LocalFree(buf);
        throw;
    }
}
#endif

unique_shared_lib::unique_shared_lib()
    : _handle{nullptr}, _error{}, _symbols{}, _curr_uid{0} {} // should we reserve?

unique_shared_lib::unique_shared_lib(unique_shared_lib&& other) noexcept {
    *this = std::move(other);
}

unique_shared_lib& unique_shared_lib::operator=(unique_shared_lib&& other) noexcept {
    if (this != &other) {
        _handle = std::move(other._handle);
        other._handle = nullptr;
        _error = std::move(other._error);
        _symbols = std::move(other._symbols);
        _curr_uid = std::move(other._curr_uid);
        other._curr_uid = 0;
    }
    return *this;
}

unique_shared_lib::~unique_shared_lib() noexcept {
    if (!_handle) {
        return;
    }
    // We ignore any errors that happen in the dtor as I don't see how we'd
    // report them.
#if defined(PLATFORM_WINDOWS)
    (void)::FreeLibrary(_handle);
#elif defined(PLATFORM_POSIX)
    (void)::dlclose(_handle);
#endif

    for (std::size_t i = 0; i < _symbols.size(); i++) {
        if (_symbols[i]) {
            _symbols[i]->invalidate();
        }
    }

}

void unique_shared_lib::open(const char* filename) {
    if (_handle) {
        close();
    }
#if defined(PLATFORM_WINDOWS)
#   if WINVER > 0x603 && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP)
    // On Windows 8 and above, we might be compiled for a Metro/UWP app where
    // we cannot use ::LoadLibrary and must instead use ::LoadPackagedLibrary
    int len = ::MultiByteToWideChar(CP_ACP, 0, filename, -1, nullptr, 0);
    auto wfilename = new wchar_t[len];
    // We should be checking for MBTWC failure somewhere around here?
    (void)::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, len);
    _handle = ::LoadPackagedLibrary(wfilename, 0);
    delete[] wfilename;
#   else
    _handle = ::LoadLibraryA(filename);
#   endif
    if (_handle == nullptr) {
        _error = reiji::get_error(::GetLastError());
    }
#elif defined(PLATFORM_POSIX)
    _handle = ::dlopen(filename, RTLD_LAZY | RTLD_GLOBAL);
    if (_handle == nullptr) {
        auto err = ::dlerror();
        _error = err ? err : _error;
    }
#endif
}

void unique_shared_lib::close() {
    if (!_handle) {
        return;
    }

#if defined(PLATFORM_WINDOWS)
    if(!::FreeLibrary(_handle)) {
        _error = reiji::get_error(::GetLastError());
    }
#elif defined(PLATFORM_POSIX)
    if (::dlclose(_handle)) {
        auto err = ::dlerror();
        _error = err ? err : _error;
    }
#endif
    _handle = nullptr;
}

unique_shared_lib::native_symbol unique_shared_lib::_symbol(const char* sym_name) {
    if (_curr_uid % 10 == 0 && _curr_uid > 0) {
        // Every so often we want to clean up the _symbols vector of _symbols
        // that got destroyed before we did
        _symbols.erase(
            std::remove(std::begin(_symbols), std::end(_symbols), nullptr),
            std::end(_symbols));
    }

#if defined(PLATFORM_WINDOWS)
    native_symbol ret = ::GetProcAddress(_handle, sym_name);
    if (ret == nullptr) {
        _error = reiji::get_error(::GetLastError());
    }
    return ret;
#elif defined(PLATFORM_POSIX)
    // This approch to error handling was taken from https://linux.die.net/man/3/dlopen
    ::dlerror();
    native_symbol ret = ::dlsym(_handle, sym_name);
    auto err = ::dlerror();
    _error = err ? err : _error;
    return ret;
#endif
}

} // kc
