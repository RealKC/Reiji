
// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>
#include <cstddef>

#include <reiji/symbol.hpp>
#include <reiji/unique_shared_lib.hpp>

namespace reiji::detail {

symbol_base::symbol_base(std::uint64_t uid, unique_shared_lib* origin)
    : _uid {uid}, _origin {origin} {
    _origin->_symbols.push_back(this);
}

void symbol_base::remove_self_from_origins_symbol_vector() noexcept {
    if (not _origin) {
        return;
    }

    auto& origin_symbols = _origin->_symbols;

    // Let's make sure none of the removed symbols exist in the _symbols vector
    // makes our life easier
    origin_symbols.erase(
        std::remove(origin_symbols.begin(), origin_symbols.end(), nullptr),
        origin_symbols.end());

    // We can binary search to find ourselves as we always push to the back of
    // the _symbols vector in unique_shared_lib
    std::size_t low = 0, high = origin_symbols.size() - 1, mid;
    while (low <= high) {
        mid = (low + high) / 2;

        if (origin_symbols[mid]->_uid > _uid) {
            high = mid - 1;
        } else if (origin_symbols[mid]->_uid < _uid) {
            low = mid + 1;
        } else {
            break;   // found ourselves!
        }
    }
    origin_symbols[mid] = nullptr;
}

}   // namespace reiji::detail
