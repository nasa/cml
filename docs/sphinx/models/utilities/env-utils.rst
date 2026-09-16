Environment Variable Utilities
++++++++++++++++++++++++++++++

.. list-table:: Revision History
   :widths: 15 30 30 50
   :header-rows: 1

   * - Version
     - Date
     - Author
     - Purpose
   * - 1
     - September 2026
     - Nino Tarantino
     - Initial version

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

________________________________________________________

Introduction
============

The Environment Variable Utilities model provides C++ interfaces to retrieve environment variables
with a variety of selectable fallback behaviors of the requested variable is not set.

________________________________________________________

Requirements
============

- **CML-ENV-UTILS-1**: The model shall provide an option to terminate the program if a requested environment variable is
  not set.
- **CML-ENV-UTILS-2**: The model shall provide an option to return a default value if a requested environment variable is
  not set.
- **CML-ENV-UTILS-3**: The model shall provide an option to throw an error if a requested environment variable is not set.
- **CML-ENV-UTILS-4**: The model shall provide a method for expanding environment variables in a user-defined string.

________________________________________________________

Model Specifications
====================

Architectural Considerations
----------------------------

Existing External Capabilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Support
^^^^^^^

No dependencies.

Model Structure
~~~~~~~~~~~~~~~

.. doxygenfile:: env_utils.hh

Mathematical Formulation
------------------------

No mathematical formulation.

________________________________________________________

User's Guide
============

Retrieving an Environment Variable
----------------------------------

Exit if the Variable is Not Set
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Retrieve an environment variable's value, or else call ``std::exit(EXIT_FAILURE)`` if the
requested value is not set:

.. code-block:: cpp

    const std::string var = getenv_or_exit("MY_ENV_VAR");

Alternatively, a user-defined exit function may be provided. This function will be called
with ``EXIT_FAILURE`` as its argument.

.. code-block:: cpp

    auto my_exit_function = [](int return_value) -> void {
        // Some user-defined logic goes here.
    };
    const std::string var = getenv_or_exit("MY_ENV_VAR", my_exit_function);


Use a Default Value if the Variable is Not Set
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Define a default value to use if the requested environment variable is not set:

.. code-block:: cpp

    const std::string var = getenv_or_default("MY_ENV_VAR", "fallback-value");


Throw an Error if the Variable is Not Set
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Retrieve an environment variable's value, or else throw a runtime error.

.. code-block:: cpp

    try {
        const std::string var = getenv_or_throw("MY_ENV_VAR");
    } catch (const std::runtime_error& err) {
        // Handle the missing variable.
    }


Expanding Environment Variables in a String
-------------------------------------------

TODO: move this over here!

Extension
---------

As the model is simply a collection of functions, users may augment it with their own
environment variable handling functions.

________________________________________________________

Verification
============

Each model requirement has associated unit tests which verify it. The unit tests are described in the
:ref:`Unit-Test Cases <test-cases>` section.

Code Coverage
-------------

See detailed coverage information `here <https://coveralls.io/github/nasa/cml?branch=main>`_.

Exceptions
----------

TODO


.. _test-cases:

Unit-Test Cases
---------------

Each requirement is verified by one or more unit test constructed using the GoogleTest framework.

+---------------------------------------------------+-------------------------+
| Test                                              | Requirement(s) Verified |
+===================================================+=========================+
| :ref:`EnvUtils.Exit <exit-test>`                  | CML-ENV-UTILS-1         |
+---------------------------------------------------+-------------------------+
| :ref:`EnvUtils.DefaultValue <default-value-test>` | CML-ENV-UTILS-2         |
+---------------------------------------------------+-------------------------+
| :ref:`EnvUtils.Throw <throw-test>`                | CML-ENV-UTILS-3         |
+---------------------------------------------------+-------------------------+


.. _exit-test:

EnvUtils.Exit
~~~~~~~~~~~~~

*Purpose*: Demonstrate that the correct environment variable data is returned if the variable exists, or else calls an
exit-handling function.

*Requirement*: Satisfactory conclusion of the test satisfies the verification of requirement **CML-ENV-UTILS-1**.

*Procedure*:

1. Set the contents of an environment variable, ``CML_ENVUTILS_TEST_VAR_EXIT_TEST``, equal to ``"test-value"``.
2. Query the contents of ``CML_ENVUTILS_TEST_VAR_EXIT_TEST`` using the :cpp:func:`getenv_or_exit` function.
3. Clear the contents of an environment variable, ``DOES_NOT_EXIST_EXIT_TEST``.
4. Query the contents of ``DOES_NOT_EXIST_EXIT_TEST`` using the :cpp:func:`getenv_or_exit` function.

*Success Criteria*: The contents retrieved from the ``CML_ENVUTILS_TEST_VAR_EXIT_TEST`` environment variable
should match what they were set to in the first step of the test. Attempting to query the contents of the
``DOES_NOT_EXIST_EXIT_TEST`` environment variable should result in an exit-handling function being called.

*Results*:

+----------------------------------------------------------------------------+---------------------------+--------+
| Test Step                                                                  | Expectation               | Result |
+============================================================================+===========================+========+
| :cpp:func:`getenv_or_exit` called with ``CML_ENVUTILS_TEST_VAR_EXIT_TEST`` | ``"test-value"`` returned | Pass   |
+----------------------------------------------------------------------------+---------------------------+--------+
| :cpp:func:`getenv_or_exit` called with ``DOES_NOT_EXIST_EXIT_TEST``        | Exit handler called       | Pass   |
+----------------------------------------------------------------------------+---------------------------+--------+

By showing that:

- Attempting to retrieve the value of an environment variable previously set in the test returns that same value
- Attempting to retrieve the value of an environment variable which was previously un-set in the test results in an
  exit-handler function being called

the test ``EnvUtils.Exit`` verifies that the model satisfies the requirement **CML-ENV-UTILS-1**.

.. _default-value-test:

EnvUtils.DefaultValue
~~~~~~~~~~~~~~~~~~~~~

*Purpose*: Demonstrate that the correct environment variable data is returned if the variable exists, or else returns a
user-defined default value.

*Requirement*: Satisfactory conclusion of the test satisfies the verification of requirement **CML-ENV-UTILS-2**.

*Procedure*:

1. Set the contents of an environment variable, ``CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST``, equal to ``"test-value"``.
2. Query the contents of ``CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST`` using the :cpp:func:`getenv_or_default` function
   with a default value of ``"DEFAULT"``
3. Clear the contents of an environment variable, ``DOES_NOT_EXIST_DEFAULT_VALUE_TEST``.
4. Query the contents of ``DOES_NOT_EXIST_DEFAULT_VALUE_TEST`` using the :cpp:func:`getenv_or_default` function
   with a default value of ``"DEFAULT"``

*Success Criteria*: The contents retrieved from the ``CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST`` environment variable
should match what they were set to in the first step of the test. Attempting to query the contents of the
``DOES_NOT_EXIST_DEFAULT_VALUE_TEST`` environment variable should result in the default value of ``"DEFAULT"`` being
returned instead.

*Results*:

+----------------------------------------------------------------------------------------+---------------------------+--------+
| Test Step                                                                              | Expectation               | Result |
+========================================================================================+===========================+========+
| :cpp:func:`getenv_or_default` called with ``CML_ENVUTILS_TEST_VAR_DEFAULT_VALUE_TEST`` | ``"test-value"`` returned | Pass   |
+----------------------------------------------------------------------------------------+---------------------------+--------+
| :cpp:func:`getenv_or_default` called with ``DOES_NOT_EXIST_DEFAULT_VALUE_TEST``        | ``"DEFAULT"`` returned    | Pass   |
+----------------------------------------------------------------------------------------+---------------------------+--------+

By showing that:

- Attempting to retrieve the value of an environment variable previously set in the test returns that same value
- Attempting to retrieve the value of an environment variable which was previously un-set in the test results in a
  specified default value being returned instead

the test ``EnvUtils.DefaultValue`` verifies that the model satisfies the requirement **CML-ENV-UTILS-2**.

.. _throw-test:

EnvUtils.Throw
~~~~~~~~~~~~~~

*Purpose*: Demonstrate that the correct environment variable data is returned if the variable exists, or else throws an
error.

*Requirement*: Satisfactory conclusion of the test satisfies the verification of requirement **CML-ENV-UTILS-3**.

*Procedure*:

1. Set the contents of an environment variable, ``CML_ENVUTILS_TEST_VAR_THROW_TEST``, equal to ``"test-value"``.
2. Query the contents of ``CML_ENVUTILS_TEST_VAR_THROW_TEST`` using the :cpp:func:`getenv_or_throw` function.
3. Clear the contents of an environment variable, ``DOES_NOT_EXIST_THROW_TEST``.
4. Query the contents of ``DOES_NOT_EXIST_THROW_TEST`` using the :cpp:func:`getenv_or_throw` function.

*Success Criteria*: The contents retrieved from the ``CML_ENVUTILS_TEST_VAR_THROW_TEST`` environment variable
should match what they were set to in the first step of the test. Attempting to query the contents of the
``DOES_NOT_EXIST_THROW_TEST`` environment variable should result in an error being thrown.

*Results*:

+-------------------------------------------------------------------------------+-------------------------------+--------+
| Test Step                                                                     | Expectation                   | Result |
+===============================================================================+===============================+========+
| :cpp:func:`getenv_or_throw` called with ``CML_ENVUTILS_TEST_VAR_THROW_TEST``  | ``"test-value"`` returned     | Pass   |
+-------------------------------------------------------------------------------+-------------------------------+--------+
| :cpp:func:`getenv_or_throw` called with ``DOES_NOT_EXIST_THROW_TEST``         | ``std::runtime_error`` thrown | Pass   |
+-------------------------------------------------------------------------------+-------------------------------+--------+

By showing that:

- Attempting to retrieve the value of an environment variable previously set in the test returns that same value
- Attempting to retrieve the value of an environment variable which was previously un-set in the test results in an
  error being throw

the test ``EnvUtils.Throw`` verifies that the model satisfies the requirement **CML-ENV-UTILS-3**.
