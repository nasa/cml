.. Reference to the next section
.. _TheCommonModelLibraryDocumentation:

The Common Model Library
========================

CML is a collection of generic, verified, validated, and reusable mathematical models of spacecraft.
The models are primarily intended for use in conjunction with NASA's
`Trick Simulation framework <https://nasa.github.io/trick/>`_.
These models are comprised of:

- :doc:`Dynamics models <src/models/dynamics/index>` which initialize or describe a spacecraft's state.
- :doc:`Environment models <src/models/environment/index>` which implement celestial body gravitational and atmospheric effects.
- :doc:`Flight hardware models <src/models/fhw/index>` which model the physics of sensors, actuators, and avionics.
- :doc:`Interaction models <src/models/interactions/index>` of a spacecraft's immediate orbital environment on its equations of motion.
- :doc:`Vehicle management models <src/models/vehicle_management/index>` for event-handling logic and flight software emulation.
- :doc:`Miscellaneous numerical utility models <src/models/utilities/index>` frequently used in spacecraft simulations.
- :doc:`Tools <src/models/tools/index>` for verification testing in a Trick simulation environment.

.. toctree::
    :maxdepth: 2
    :caption: Models
    :name: models
    :titlesonly:
    :hidden:

    src/models/dynamics/index
    src/models/environment/index
    src/models/fhw/index
    src/models/interactions/index
    src/models/vehicle_management/index
    src/models/tools/index
    src/models/utilities/index

.. toctree::
    :maxdepth: 1
    :caption: Contributing
    :name: contributions
    :titlesonly:
    :hidden:

    src/contributing/getting-started
    src/contributing/contributing
    src/contributing/coding-standards
    src/contributing/building-the-docs

.. toctree::
   :maxdepth: 1
   :caption: Helpful Links
   :name: helpful-links
   :titlesonly:
   :hidden:

   GitHub Repository <https://github.com/nasa/cml>
   Trick Documentation <https://nasa.github.io/trick>
   JEOD Documentation <https://github.com/nasa/jeod/tree/main/docs>

________________________________________________________

Using CML in Your Project
-------------------------

Clone CML from its source in GitHub, optionally with a release tag specified. We recommend
that you include CML in your project as a submodule.

.. code-block:: shell

   # Clone with ssh:
   git submodule add -b RELEASE_TAG git@github.com:nasa/cml.git cml
   # Clone with HTTPS:
   git submodule add -b RELEASE_TAG https://github.com/nasa/cml.git cml

.. tip::

   Official releases of CML can be found at the `Releases page <https://github.com/nasa/cml/releases>`_.
   We recommend that you use an official release of CML because all models have been verified at that point.
   Intermediate commits may contain changes that have not yet been formally verified in accordance with NASA-STD-7009.

CML provides a shared ``cml.mk`` file which you can include in your Trick sim's
``S_overrides.mk`` file. Replace ``${CML_HOME}`` with the path to the root of your CML submodule.
It is not necessary to define that environment variable to use CML.

.. code-block:: makefile

   include "${CML_HOME}/mkspecs/cml.mk"

Users have control over the build options. CML supports pre-compiling all of CML
into a ``libcml`` library and `Trickifying CML <https://nasa.github.io/trick/documentation/building_a_simulation/Trickified-Project-Libraries.html>`_
to provide very fast compilation times to consuming projects. Build options are specified
by defining variables prior to including ``cml.mk``. For a full list of these options
and an overview of the default behavior, see the header comment in `cml.mk <https://github.com/nasa/cml/blob/main/mkspecs/cml.mk>`_.

When including a CML model, its include path should be specified relative to the directory
in which CML itself lives. However, note that this is planned to change in the future.

.. code-block:: cpp

   #include "cml/models/path/to/header.hh"

________________________________________________________

Model Maturity Levels
---------------------

Within each model's README.md file, the model's maturity level is specified. This is a somewhat deprecated
system as we're currently working towards getting all models to a "level 5" maturity, but in the meantime
should serve as a general overview of the level of work that has gone into verification and validation of
the model in question.

0. Either the model is obsolete or incomplete. Typically is well developed conceptually but may not build
   or run as intended. Model should be used as reference only. **No models in CML should be at level 0.**
1. Model builds and runs. Typically lacks documentation and verification. Model may be used, but at your
   own risk. **No models in CML should be at level 1.**
2. Model has undergone some preliminary review and/or verification but unit testing is minimal to non-existent.
   Documentation may be started, but generally lacking in detail and reliability. Model may be used, with
   caution.
3. Model has substantial documentation and reliable verification test cases that can be implemented as part
   of the project's unit-test framework. Model has not been independently verified.
4. Model development is complete, including comprehensive verification and documentation. Independent
   verification has not been completed. Level-4 may indicate a previously-level-5 model has since been edited
   and is awaiting a delta-verification review.
5. Model is mature, it has been comprehensively documented and independently verified.