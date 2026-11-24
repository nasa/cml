#############
CML Namespace
#############

.. doxygennamespace:: cml
    :desc-only:

All CML code is placed within the ``cml`` C++ namespace. Users should avoid placing any model
code in ``cml::`` to avoid name collisions.

Additionally, all Python code is played in the `cml` module. When using a function, type, or any
other SWIG binding for CML, users should access it as follows:

.. code-block:: python

    trick.cml.some_function(args)
    some_instance = trick.cml.SomeClass()
