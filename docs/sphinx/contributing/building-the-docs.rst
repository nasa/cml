Building the Documentation
==========================

Our documentation is written using:

* `Doxygen <https://www.doxygen.nl/>`_ to generate API information about the models.
* `reStructuredText <https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html>`_ as the markup language.
* `Sphinx <https://www.sphinx-doc.org/en/master/index.html>`_ to convert the documentation into HTML.
* `Breathe <https://breathe.readthedocs.io/en/latest/>`_ to transpile the Doxygen output into a format that Sphinx can
  use.

Of these dependencies, Doxygen is the only one that isn't included automatically in the CML virtual environment. It will
need to be installed on your system.

.. _initial-build:

The Initial Build
-----------------

Follow our :doc:`Getting Started Guide <getting-started>` to set up a virtual environment, then activate it.

.. code-block:: bash

    activate ${CML_HOME}/.venv/bin/activate

If you're a CML developer, you can run the ``CMake`` ``dev`` preset to build all of CML and run unit tests, and then
manually build the documentation.

.. code-block:: bash

    cmake --workflow --preset dev
    cmake --build --preset dev -t documentation

If you're simply trying to build only the documentation, you can run the ``documentation`` preset instead.

.. code-block:: bash

    cmake --workflow --preset documentation

Regardless of which preset you choose, the documentation will be placed into the following directory structure:

* ``build/docs/doxygen``: Generated XML metadata of all CML classes and functions.
* ``build/docs/sphinx``: The HTML source that can be opened in your browser.

To view the documentation, use your browser of choice to open the ``index.html`` file in the Sphinx build directory.

.. code-block:: bash

    firefox build/docs/sphinx/index.html


Editing the Documentation
-------------------------

When live-editing the documentation, the recommended tool is `sphinx-autobuild <https://github.com/sphinx-doc/sphinx-autobuild>`_.
``sphinx-autobuild`` is provided by our virtual environment. After :ref:`generating the documentation <initial-build>`, run the
``livehtml`` target in our Sphinx makefile, which is a wrapper around ``sphinx-autobuild`` with all the necessary options
automatically provided.

.. code-block:: bash

    cd docs/sphinx
    make livehtml

This will start a server and watch for changes to any of the reStructuredText files that make up the CML documentation,
automatically rebuilding the documentation when you save your changes.

.. note::
    If you edit the CML source code (headers or source files), you'll need to re-run the CMake step above. This is because
    the XML output needs to be re-generated, which ``sphinx-autobuild`` does **not** do automatically, nor will it automatically
    detect changes to any of the XML files.

.. important::
    If you create a new .rst file under the ``docs`` directory, please be sure to update ``docs/CMakeLists.txt`` with the relative
    path to that file. This allows CMake to determine that it should re-invoke ``sphinx-build`` the next time you build CML or
    the documentation.

See the `reStructuredText documentation <https://www.sphinx-doc.org/en/master/usage/restructuredtext/index.html>`_ for examples
and references on writing reStructuredText. If you prefer, you may also choose to write documentation in Markdown and convert it
to reStructureText using tools like ``pandoc``.