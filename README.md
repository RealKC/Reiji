# Reiji

![Windows](https://github.com/RealKC/Reiji/workflows/Windows/badge.svg) ![Linux](https://github.com/RealKC/Reiji/workflows/Linux/badge.svg) ![MacOS](https://github.com/RealKC/Reiji/workflows/MacOS/badge.svg)

Reiji is a C++ library that abstracts away the platform-specific code for loading
a shared library at runtime.

You may read Reiji's documentation [here](https://RealKC.github.io/Reiji).

## Design goals

Reiji was designed with the following goals in mind:

* **safety**: using the `symbol<T>` proxy, Reiji makes it hard to trigger
potentially undefined behaviour after a shared library was unloaded
* **an intuitive interface**: opening a shared library feels as natural as
opening a `std::ifstream`, the `symbol<T>` proxy makes working with symbols taken
from shared libraries as easy as working with pointers.

## Integration

Reiji uses CMake, so you can simply add this repository as a submodule and add
the following lines to your CMakeLists.txt

```cmake
add_subdirectory(path/to/reiji)
target_link_libraries(<my target> Reiji::Reiji)
```

## Contributing

If you wish to contribute, you may do by submitting bug reports or pull requests.

When writing code, remember that Reiji is written in C++17, and that code
formatting is done using `clang-format`.

## Legal stuff

Reiji is licensed under the [Boost Software License](LICENSE.txt)(alternatively,
you may read it [here](https://www.boost.org/LICENSE_1_0.txt)).

Reiji uses the [doctest library](https://github.com/onqtam/doctest) licensed under
the MIT license for its tests.
