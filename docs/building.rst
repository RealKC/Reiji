Building Reiji
==============

.. contents:: :local:


Building the library
====================

To build Reiji you will need git, cmake, and a C++17 compiler that can offers
the ``<filesytem>`` library.

.. code-block:: sh

   git clone https://github.com/RealKC/Reiji.git
   cd Reiji
   mkdir build && cd build
   cmake \.\.
   make

Building the documentation
==========================

To build the documentation you will also need :doxygen:`Doxygen <>`,
:sphinx:`Sphinx <>`, and :breathe:`breathe <>`.

Assuming you have already ran the previous steps, you only need to run the
following command:

.. code-block:: sh

   make documentation
