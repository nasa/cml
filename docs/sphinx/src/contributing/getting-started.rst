Getting Started
===============

This guide is primarily intended for developers and other contributors to CML. If you're only intending
to use CML, following these instructions is not necessary.

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

First-time Setup
----------------

The following steps only need to be run once to set up your environment for working in CML.

.. _virtual-environment:

Setting up a Virtual Environment
++++++++++++++++++++++++++++++++

The scripts which manage testing CML content leverage `TrickOps <https://github.com/nasa/trick/tree/master/share/trick/trickops>`_
modules which themselves require valid ``python3`` virtual environments to be created and activated with
content defined by ``${TRICK_HOME}/share/trick/trickops/requirements.txt``. Additionally, CML model input files
and various static analysis tools require non-system packages like ``numpy`` and ``gcovr``.

To create and manage your virtual environment, CML recommends using `uv <https://docs.astral.sh/uv/>`_.

.. code-block:: shell

    uv sync
    source .venv/bin/activate

If you don't have ``uv`` installed, you can use ``pip``.

.. code-block:: shell

    python3 -m venv .venv
    source .venv/bin/activate
    pip3 install --upgrade pip
    pip install .


Configuring Your Environment
++++++++++++++++++++++++++++

CML requires several environment variables to be defined to properly configure some of the tools we use for
development, such as address sanitizer suppressions. These variables are defined in the root level ``.bashrc``
file. Simply run the following command to activate it.

.. code-block:: shell

    source .bashrc

.. important::

    Running this command should be the first thing you do when opening a new terminal that you're going to use for
    CML development.

Note that the ``.bashrc`` file will print warnings if ``${TRICK_HOME}`` and ``${JEOD_HOME}`` are not defined. See
the :ref:`Trick and JEOD installation instructions <trick-and-jeod>` for more information. It will also look for
the existence of the virtual environment as described in the :ref:`virtual environment section <virtual-environment>`.
Also be aware that your environment should be free of any Trick-related environment variables such as ``${TRICK_CFLAGS}``
and ``${TRICK_CXXFLAGS}`` before sourcing the ``.bashrc`` file.

You may prefer to set up an alias in your home directory ``~/.bashrc`` to quickly set up your CML environment for
development. For example:

.. code-block:: bash

    alias cml='export TRICK_HOME=/<path_to>/trick; export JEOD_HOME=/<path_to>/jeod; source /<path_to>/cml/.bashrc'

As previously mentioned, the ``.bashrc`` script is purely intended for developer use. While it probably won't hurt,
we don't recommend that external projects or users run this script. It may override some settings that your project
has set up that clash with CML's internal development tools.

.. _trick-and-jeod:

Installing Trick and JEOD
+++++++++++++++++++++++++

In order to use CML models, your workflow must have access to compatible versions of Trick and JEOD. The ``${TRICK_HOME}``
and ``${JEOD_HOME}`` environment variables must point to valid installations of Trick and JEOD, respectively. Currently,
we require:

-  `Trick 25.1.0 <https://github.com/nasa/trick>`_ or newer.
-  `JEOD 5.4.1 <https://github.com/nasa/jeod>`_ or newer.

.. note::

    We recommend building Trick after setting up your :ref:`virtual environment <virtual-environment>`. Doing so will direct
    Trick to use the version of Python which includes CML dependencies for input processing.

.. important::

    As part of our testing pipeline, we compare the logged data output from verification simulations to the committed Baseline
    data. It is possible that using a newer version of JEOD or a different operating system than what the verification
    data was generated with will result in small changes to verification simulation outputs. Keep this in mind when running unit
    tests.

To build Trick, we recommend cloning it into the ``externals/`` directory.

.. code-block:: shell

    source .bashrc  # To activate the virtual environment
    git clone https://github.com/nasa/trick --branch <TRICK-VERSION> ${CML_HOME}/externals/trick
    cd ${CML_HOME}/externals/trick
    ./configure
    make -j$(nproc)

To build JEOD, we recommend also cloning it into the ``externals/`` directory.

.. code-block:: shell

    git clone https://github.com/nasa/jeod --branch <JEOD-VERSION> ${CML_HOME}/externals/jeod
    cd ${CML_HOME}/externals/jeod
    make -f bin/jeod/makefile -j$(nproc) TRICK_BUILD=1
    make -C trickified -j$(nproc)

Building CML
------------

We use `CMake <https://cmake.org/>`_ as our meta-build tool. You can use our pre-configured ``dev`` workflow
preset to build CML and run the unit tests in a single command.

.. code-block:: shell

    cmake --workflow --preset dev

This will generate build scripts in the ``${CML_HOME}/build/debug`` directory, build CML, and run unit tests.
It will also install the CML libraries into ``${CML_HOME}/lib``.

Running Verification Simulations
--------------------------------

A majority of our models are testing in a Trick environment. You can build, run, and analyze verification
simulation outputs using the ``bin/test.py`` script.

.. code-block:: shell

    bin/test.py

Use the ``-h`` option to see a full list of arguments accepted by the script. The script allows you to specify things
like which stages to test (building, running, comparing verification data, etc) and how many parallel processes
should run.