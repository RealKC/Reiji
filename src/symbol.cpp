
// Copyright Mițca Dumitru 2020 - present
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt)

#include <cstddef>

#include <reiji/symbol.hpp>
#include <reiji/unique_shared_lib.hpp>

namespace reiji::detail {

void symbol_base::remove_self_from_origins_symbol_vector() noexcept {
    if (not _origin) {
        return;
    }

    // Let's make sure none of the removed symbols exist in the _symbols vector
    // makes our life easier
    _origin->_symbols.erase(
        std::remove(_symbols.begin(), _symbols.end(), nullptr), _symbols.end());

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
            break;   // found ourselves!
        }
    }
    _origin->_symbols[mid] = nullptr;
}

}   // namespace reiji::detail
