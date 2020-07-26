Configuration macros
====================

Reiji will attempt to discover whether exceptions are supported or not, and
define :ref:`REIJI_ON_INVALID_SYMBOL <reiji_on_invalid_symbol>` accordingly,
however you may specify the error reporting mechanism Reiji should use using
either :ref:`REIJI_SYMBOL_THROW_EXCEPTIONS <reiji_symbol_throw_exceptions>`
or :ref:`REIJI_SYMBOL_ABORT <reiji_symbol_abort>`.

Alternatively, you may define :ref:`REIJI_ON_INVALID_SYMBOL <reiji_on_invalid_symbol>`
yourself before including any Reiji headers.

.. _reiji_on_invalid_symbol:
.. doxygendefine:: REIJI_ON_INVALID_SYMBOL

.. _reiji_symbol_throw_exceptions:
.. doxygendefine:: REIJI_SYMBOL_THROW_EXCEPTIONS

.. _reiji_symbol_abort:
.. doxygendefine:: REIJI_SYMBOL_ABORT
