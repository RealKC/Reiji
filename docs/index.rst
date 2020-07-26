.. Reiji documentation master file, created by
   sphinx-quickstart on Mon Jul 20 20:59:29 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Welcome to Reiji's documentation!
=================================

.. toctree::
   :maxdepth: 2
   :caption: Contents:

   building
   unique_shared_lib
   symbol
   macros

.. image:: https://github.com/RealKC/Reiji/workflows/Windows/badge.svg

.. image:: https://github.com/RealKC/Reiji/workflows/Linux/badge.svg

.. image:: https://github.com/RealKC/Reiji/workflows/MacOS/badge.svg

Reiji is a C++17 library made for easy & safe loading of shared libraries at
runtime and getting symbols from them.

Example:

.. code-block:: c++

   reiji::unique_shared_lib lib {"foo.so"};
   auto bar = lib.get_symbol<void()>("bar");
   bar();
   auto baz = lib.get_symbol<int>("baz");
   std::cout << *baz;


Supported platforms
* Windows
  * Win32 applications
  * UWP/Metro
* POSIX platforms(MacOS, Linux, BSDs, etc.)

Integrating Reiji into a project
================================
Reiji uses CMake, so you can simply add this repository as a submodule and add
the following lines to your CMakeLists.txt

.. code-block:: cmake

   add_subdirectory(path/to/reiji)
   target_link_libraries(<my target> Reiji::Reiji)
