
// Copyright Mițca Dumitru 2019 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

// Disable clang-format so it doesn't reorder these headers
// clang-format off
#include <reiji/unique_shared_lib.hpp>
#include <reiji/detail/push_platform_detection_macros.hpp>
// clang-format on

#if REIJI_PLATFORM_WINDOWS
#    include <cstddef>   // std::size_t
#    include <new>
#    include <windows.h>
#elif REIJI_PLATFORM_POSIX
#    include <dlfcn.h>
#endif

#include <algorithm>   // std::remove
#include <utility>     // std::move, std::exchange

namespace reiji {

#if REIJI_PLATFORM_WINDOWS
static inline std::string get_error(DWORD err_code) {
    struct deferred_local_free final {
        deferred_local_free(void* p) noexcept : _p {p} {}
        ~deferred_local_free() noexcept { ::LocalFree(_p); }

    private:
        void* _p;
    };

    ::LPSTR buf     = nullptr;
    std::size_t len = ::FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
            | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, err_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (::LPSTR)&buf, 0, nullptr);
    deferred_local_free dlf {buf};

    std::string error {buf, len};
    return error;
}
#endif

unique_shared_lib::unique_shared_lib(unique_shared_lib&& other) noexcept {
    *this = std::move(other);
}

unique_shared_lib&
unique_shared_lib::operator=(unique_shared_lib&& other) noexcept {
    if (this != &other) {
        close();
        _handle   = std::exchange(other._handle, nullptr);
        _error    = std::move(other._error);
        _symbols  = std::move(other._symbols);
        _curr_uid = std::exchange(other._curr_uid, 0);
    }
    return *this;
}

unique_shared_lib::~unique_shared_lib() noexcept {
    close();
}

void unique_shared_lib::open(const char* filename) {
#if REIJI_PLATFORM_WINDOWS
    // FIXME: Make open(name, flags) care for flags on windows.
    open(filename, 0);
#elif REIJI_PLATFORM_POSIX
    open(filename, RTLD_LAZY | RTLD_GLOBAL);
#endif
}

void unique_shared_lib::open(const char* filename, flags_type flags) {
    if (_handle) {
        close();
    }
#if REIJI_PLATFORM_WINDOWS
#    if WINVER > 0x602 && WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_APP) && !WINAPI_FAMILY_PARTITION(WINAPI_PARTITION_DESKTOP)
    // On Windows 8 and above, we might be compiled for a Metro/UWP app where
    // we cannot use ::LoadLibrary and must instead use ::LoadPackagedLibrary
    int len        = ::MultiByteToWideChar(CP_ACP, 0, filename, -1, nullptr, 0);
    auto wfilename = new wchar_t[len];
    // We should be checking for MBTWC failure somewhere around here?
    (void)::MultiByteToWideChar(CP_ACP, 0, filename, -1, wfilename, len);
    _handle = ::LoadPackagedLibrary(wfilename, 0);
    delete[] wfilename;
#    else
    _handle = ::LoadLibraryA(filename);
#    endif
    if (not _handle) {
        _error = reiji::get_error(::GetLastError());
    }
#elif REIJI_PLATFORM_POSIX
    _handle = ::dlopen(filename, flags);
    if (not _handle) {
        auto err = ::dlerror();
        _error   = err ? err : _error;
    }
#endif
}

void unique_shared_lib::close() {
    if (not _handle) {
        return;
    }

    _error = "";

#if REIJI_PLATFORM_WINDOWS
    if (not ::FreeLibrary(_handle)) {
        _error = reiji::get_error(::GetLastError());
    }
#elif REIJI_PLATFORM_POSIX
    if (::dlclose(_handle)) {
        auto err = ::dlerror();
        _error   = err ? err : "";
    }
#endif
    _handle = nullptr;

    for (std::size_t i = 0; i < _symbols.size(); i++) {
        if (_symbols[i]) {
            _symbols[i]->_invalidate();
        }
    }
    _symbols.clear();
    _curr_uid = 0;
}

void unique_shared_lib::swap(unique_shared_lib& other) {
    using std::swap;
    swap(_handle, other._handle);
    swap(_curr_uid, other._curr_uid);
    swap(_error, other._error);
    swap(_symbols, other._symbols);
}

unique_shared_lib::native_symbol
unique_shared_lib::_get_symbol(const char* sym_name) {
    if (not _handle) {
        using namespace std::string_literals;
        _error = "Cannot load symbol '"s + sym_name
                 + "' when no library was opened.";
        return nullptr;
    }

    if (_curr_uid % 10 == 0 && _curr_uid > 0) {
        // Every so often we want to clean up the _symbols vector of _symbols
        // that got destroyed before we did
        _symbols.erase(std::remove(_symbols.begin(), _symbols.end(), nullptr),
                       _symbols.end());
    }

#if REIJI_PLATFORM_WINDOWS
    native_symbol ret = ::GetProcAddress(_handle, sym_name);
    if (not ret) {
        _error = reiji::get_error(::GetLastError());
    }
    return ret;
#elif REIJI_PLATFORM_POSIX
    // This approch to error handling was taken from
    // https://linux.die.net/man/3/dlopen
    ::dlerror();
    native_symbol ret = ::dlsym(_handle, sym_name);
    auto err          = ::dlerror();
    _error            = err ? err : _error;
    return ret;
#endif
}

}   // namespace reiji
