``unique_shared_lib`` - a cross platform abstraction over low level APIs
========================================================================

``unique_shared_lib`` represents the high level concept of a shared library
loaded at runtime, abstracting the low level APIs provided by Windows and
POSIX operating systems.

To ensure safety, an object of type ``unique_shared_lib`` remembers the
:doc:`symbols <symbol>` it hands out and invalidates them once it gets
destroyed.

.. doxygenclass:: reiji::unique_shared_lib
   :members:

.. doxygenfunction:: reiji::swap(unique_shared_lib&, unique_shared_lib&)
