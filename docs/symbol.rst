Symbols in Reiji
================

Reiji's main safety-providing abstraction is the ``symbol<T>`` class.

``symbol<T>`` is a pointer-like proxy that ties a symbol you get from a shared
library to that library's lifetime. When that library goes out of scope, your
symbols will become invalid and you will be prevented through runtime mechanisms
from triggering undefined behaviour.

Throughout this docu ment the term ``invalid`` is used to refer to a symbol on
which calling ``operator*``, ``operator->`` (for ``symbol<T>``) or ``operator()``
(for ``symbol<R(As...)>``) will trigger :ref:`REIJI_ON_INVALID_SYMBOL <reiji_on_invalid_symbol>` .

.. doxygenclass:: reiji::symbol
   :members:

.. doxygenclass:: reiji::symbol< R(Args...)>
   :members:

.. doxygenfunction:: reiji::operator==(std::nullptr_t, const symbol< T>&)

.. doxygenfunction:: reiji::operator!=(const symbol<T>&, const symbol<T>&)

.. doxygenfunction:: reiji::operator<=(const symbol<T>&, const symbol<T>&)

.. doxygenfunction:: reiji::operator>=(const symbol<T>&, const symbol<T>&)

.. doxygenfunction:: reiji::swap(symbol<T>&, symbol<T>&)

.. doxygenclass:: reiji::bad_symbol_access

A free function ``reiji::swap`` and symetric inequality operators are also
defined for ``symbol<T>``.
