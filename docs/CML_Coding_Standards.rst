==================
 Coding Standards
==================

.. list-table:: Revision History
   :widths: 30 30 70

   * - Identifier
     - Release date
     - Description
   * - v0.1 (Baseline)
     - 03/31/2026
     - Initial CML coding standards
   * - v0.2
     - 07/21/2026
     - Changed GitLab references to GitHub and converted to reStructuredText

.. list-table:: Approvers
   :widths: 150 150

   * - Prepared by
     - Approved by
   * - | Nino Tarantino
       | CML Maintainer
       | 03/31/2026
     - | Daniel Jordan
       | CML Lead
       | 03/31/2026

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

Introduction
============

This document provides the formal code standards that are used within the Common Model Library (CML) team. The coding
standards in this document are designed to protect the reliability, maintainability, and usability of the library while
avoiding overly restrictive standards that may prevent some simulation teams from easily contributing their models to
CML. These standards are written based on generally accepted best practices in industry as well as lessons learned from
experienced developers in the Trick-based simulation field at NASA JSC. These standards are expected to mature and
develop over time.

Purpose
-------

Coding standards are used in large software development projects because they provide multiple benefits to the
organization developing the software application. These benefits are seen as critical for the CML team developing and
maintaining complex and high-fidelity time-domain simulation models used for spacecraft analysis.

Scope
-----

The coding standards listed in this document apply only to the developed code associated with models under the control
of the CML team. These coding standards do not apply to external packages such as Trick, JEOD, or any other third-party
dependency.

Change Authority/Responsibility
-------------------------------

Proposed changes to this document shall be submitted as a GitHub issue or via email to the CML team for consideration
and disposition with the team lead.

Applicable Documents
--------------------

The following documents include specifications, models, standards, guidelines, handbooks, and other special
publications. The documents listed in this product are applicable to the extent specified herein.

.. list-table::
   :widths: 30 30 70
   :header-rows: 1

   * - Document Number
     - Document Revision
     - Document Title
   * - NASA-STD-7009
     - A.1
     - Standard for Models and Simulations
   * - NPR 7150.2
     - C
     - NASA Software Engineering Requirements

Reference Documents
-------------------

The following documents contain supplemental information to guide the user in the application of this document.

.. list-table::
   :widths: 50 50 50

   * - Document Number
     - Document Revision
     - Document Title
   * -
     -
     -

Coding Standards
================

This section describes the coding standards and how they apply to the software development workflow. The purpose of
having coding standards is to ensure that a project's code base not only functions correctly but is also readable
modifiable, and maintainable. The code that comes into the library must be easily reviewable and not threaten the
stability of the library itself. With these goals in mind, the CML Coding Standards fall in two categories:

-  :ref:`model-coding-standards` – These coding standards should be met upon a developer’s Pull
   Request Review. These standards are intended to ensure the quality, stability, and cohesiveness of CML.
   Non-compliances will be identified in the review and evaluated for risk to integration into the library.
-  :ref:`best-practice-coding-standards` – These coding standards should be met as early as
   possible but are not required to be met upon a developer’s Pull Request Review. These standards are intended to
   ensure that the codebase follows modern best practices. Non-compliances will be evaluated for risk to the library and
   documented in a follow-on GitHub issue for future resolution if necessary.

Application and Enforcement
---------------------------

New models submitted to CML must first pass through a Pull Request Review. During the review, the submitting group
provides CML with the NASA NPR 7150.2 classification of the model and evidence of NASA-STD-7009 compliance. The CML team
will review the documentation provided and will check the models submitted for adherence to the coding standards. Many
of the model coding standards are checked with assistance from automated tooling, which reduces the amount of manual
review necessary.

Updates to existing CML code will also be checked for adherence to the coding standards during a Pull Request Review.
Unlike a new model submission, NPR 7150.2 and NASA-STD-7009 documentation is not required unless the model’s
classification has changed.

.. _model-coding-standards:

Model Coding Standards
----------------------

These standards cover the attributes of the source code required for CML models and must be met before acceptance of the
source code into the main branch of the CML codebase.

Model Coding Standards are broken into categories depending on the language used. CML-MCS-GENERAL standards apply to all
code, regardless of language. CML-MCS-CPP standards apply to C++ code. CML-MCS-PYTHON standards apply to Python code.

**CML-MCS-GENERAL-1**: All Trick-based simulation model code shall be written in the C++ programming language
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Trick simulation models should be written in C++. Other programming languages, such as C, are not allowed
to be used for model implementation. Autocoded models from MATLAB or Simulink source are not considered to have been
written in C++.

**Enforcement**: Static analysis and Pull Request Review.

**CML-MCS-GENERAL-2**: All scripts required to maintain model code must be committed at the time of Pull Request Review
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Some models may contain data or other content that was automatically generated by means of a script.
These scripts must also be committed and live alongside the model so that the model may be properly maintained.

**Enforcement**: Pull Request Review.

**CML-MCS-GENERAL-3**: Source code shall not contain ITAR, EAR, or CUI data
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: CML is an open-source project and must not contain sensitive data.

**Enforcement**: Pull Request Review.

**CML-MCS-GENERAL-4**: Source code shall be formatted in accordance with a common stylesheet
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Consistent codebases are easier to use and maintain. Automated code formatting ensures that the git
history contains only impactful changes to the code rather than formatting tweaks. The specific formatting
enforcement tool and stylesheet may differ between languages.

**Enforcement**: Static analysis operating off of a common stylesheet available to developers and users.

**CML-MCS-GENERAL-5**: Commented-out code shall only be allowed when accompanied by an associated project issue, point of contact email address, and current date
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Commented-out code should be rare. When it is included in the codebase, there should be an expectation
that the code will be un-commented or removed by a certain date.

**Enforcement**: Pull Request Review.

**Example**

.. code-block:: cpp

    // TODO jane.doe@nasa.gov 01/01/2000: uncomment when addressing issue #1.
    //my_type.nonexistent_field = 1;

**CML-MCS-CPP-1**: Source code shall live within the “cml” C++ namespace
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Namespaces prevent name collisions between CML-defined types and the types defined by consumers of the
CML library.

**Enforcement**: Pull Request Review.

**Example**

.. code-block:: cpp

   namespace cml {

   struct MyType {
   };

   void my_function(int my_param);

   }

**CML-MCS-CPP-2**: Header files shall direct Trick to place generated interface code in the “cml” Python module via the Trick header “PYTHON MODULE” directive
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Trick provides the option to generate Python interface code into a specific Python module rather than in
the global “trick” scope. This prevents name collisions between CML-defined types and the types defined by Trick or
consumers of the CML library.

**Enforcement**: Static analysis.

**Example**

.. code-block:: cpp

   /* PURPOSE: (My purpose)
    *
    * PYTHON_MODULE: (cml)
    */

**CML-MCS-CPP-3**: Source code shall use the .hh file extension for headers and the .cc file extension for source files
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Consistent codebases are easier to use and maintain.

**Enforcement**: Static analysis.

**CML-MCS-CPP-4**: Functions and types limited in scope to the implementation details of a module shall be placed in an unnamed namespace within the corresponding module source file
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Exposing internal implementation details in public headers slows down Trick interface code generation and
may confuse users. Unnamed namespaces are the modern equivalent to static functions and types declared at the file-level.

**Enforcement**: Pull Request Review.

**Example**

.. code-block:: cpp

   // Within myfile.cc
   namespace {

   void some_local_function() {
   }

   }

**CML-MCS-CPP-5**: Structs shall be used only for passive objects with all public fields that have no invariants
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Structs are widely understood by the C++ community to carry data with no invariants. Although structs may
have protected or private fields, if such encapsulation is necessary, then classes should be used.

**Enforcement**: Pull Request Review.

**Examples**

.. code-block:: cpp

   struct JustSomeData {
       int int_data {};
       double double_data {1.0};
       std::string string_data {"Some Data"};
   };

   class EncapsulationNecessary {
   public:
       void set_coefficient_of_friction(double coefficient_of_friction_in) {
           if (coefficient_of_friction_in >= 0.0) {
               coefficient_of_friction = coefficient_of_friction_in;
           } else {
               throw_some_error();
           }
       }

   private:
       double coefficient_of_friction {};
   };

**CML-MCS-CPP-6**: Source code shall compile with no warnings
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Compiler warnings signify that something may be wrong with the code and should rarely be ignored.

**Enforcement**: Automated builds in GitHub Actions treat warnings as errors.

**CML-MCS-CPP-7**: Source code shall pass review by linters without generating any warnings
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Linters such as ``cppcheck`` and ``clang-tidy`` are widely used in industry and provide meaningful
feedback on potential code quality issues. These warnings should rarely be ignored.

**Enforcement**: Linters are run in a GitHub Action and produce a failing result upon detecting an error.

**CML-MCS-CPP-8**: Source code shall have associated unit tests which cover at least 90% of lines with meaningful tests
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Inadequately tested code is prone to introducing regressions and is not fit for use within high-fidelity
spacecraft simulations.

**Enforcement**: Manual review of code coverage artifacts generated in a GitHub Action during Pull Request Review.

**CML-MCS-CPP-9**: Source code shall compile using the ISO/IEC 14882 C++17 standard
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: A reasonably recent version of the C++ language should be used by all models to leverage new language
features and maintain interoperability with other open-source libraries.

**Enforcement**: All toolchains use the minimum supported C++ standard.

**CML-MCS-CPP-10**: Source code shall have associated documentation which includes, at minimum, the model’s public API, a users’ guide, assumptions and limitations, and details about prior verification and validation activities
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Code without documentation cannot be effectively maintained or used.

**Enforcement**: Pull Request Review.

**CML-MCS-PYTHON-1**: Source code shall not call exec(), eval(), or otherwise evaluate an arbitrary string as Python code
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Exceptions thrown within code evaluated by ``exec()`` or other related functions do not provide meaningful
debugging information. These functions also introduce security concerns into the codebase. Use modules and functions
instead of ``exec(open())``.

**Enforcement**: Pull Request Review.

**Example, wrong**

.. code-block:: python

   exec(open("Modified_data/utils.py").read())
   some_function_from_utils()

**Example, correct**

.. code-block:: python

   from Modified_data.utils import some_function_from_utils
   some_function_from_utils()

**CML-MCS-PYTHON-2**: Methods which accept keyword arguments shall document all allowable keyword arguments
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Users should not have to read implementation details to determine which keyword arguments are accepted by
a function.

**Enforcement**: Pull Request Review.

**Example**

.. code-block:: python

   def myfunc(*args, **kwargs) -> None:
       """Does a few things.

       Keyword arguments:
       ------------------
       foo : An instance of Foo
       bar : A numeric type such as an int or float
       baz : A string-like type
       """
       implementation(*args, **kwargs)

.. _best-practice-coding-standards:

Best Practice Coding Standards
------------------------------

These standards cover best practice for the code within CML. These standards are not required to be met at the time of
integration into the main CML branch but should be met as soon as possible afterwards. Like the
:ref:`model-coding-standards`, CML Best Practice Coding Standards are grouped by programming language.

**CML-BP-CPP-1**: Do not define macros
++++++++++++++++++++++++++++++++++++++

**Rationale**: Macros cannot be put into a namespace and can be difficult to debug. Use function templates and inline
variables instead.

**Example, wrong**

.. code-block:: cpp

   #define ADD_ONE(input) (input + 1)

   #define SPEED_OF_LIGHT 299792458.0

**Example, correct**

.. code-block:: cpp

   template <typename Type>
   constexpr Type add_one(const Type& input) {
      return input + 1;
   }

   inline constexpr double SPEED_OF_LIGHT = 299792458.0;

**CML-BP-CPP-2**: Do not invoke undefined behavior
++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: The outputs of a program which invokes undefined behavior are suspect.

**Note**: Most undefined behavior will be caught by static analysis tools and will be required to be addressed during
Pull Request Review.

**CML-BP-CPP-3**: Do not use global variables
+++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Global variables often lead to “spaghetti code” which is difficult to maintain and understand.

**CML-BP-CPP-4**: Do not use the “new” or “delete” keywords
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: The Trick Memory Manager should be used to allocate memory which must be logged or otherwise available
for Trick-specific purposes. Otherwise, smart pointers provided by the C++ standard library should be used for
dynamic memory management. Both of these options provide automated cleanup of the allocated memory.

**Example, wrong**

.. code-block:: cpp

   double* my_vector = new double[num_elements];

**Example, correct** using the Trick memory manager

.. code-block:: cpp

   auto* my_vector = static_cast<double*>(trick_TMM->declare_var("double", num_elements));

**Example, correct** using smart pointers

.. code-block:: cpp

   auto my_vector = std:make_unique<double[]>(num_elements);

**CML-BP-CPP-5**: Do not allocate significant memory in scheduled simulation jobs
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Trick scheduled jobs are often intended to be run at high frequencies for high-fidelity spacecraft
analysis. Frequently reallocating memory during such jobs introduces performance bottlenecks and is a symptom of bad
design. Some reallocation may be expected during certain simulation events, but most memory allocation should occur
during simulation initialization.

**CML-BP-CPP-6**: Favor composition over inheritance
++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Frequent use of inheritance can make a codebase difficult to understand. Often, composition allows for
easier code maintenance and testability.

**CML-BP-PYTHON-1**: Use docstrings
+++++++++++++++++++++++++++++++++++

**Rationale**: Python docstrings help users use Python code correctly and are recommended by PEP 257.

**Example**

.. code-block:: python

   def foo(bar) -> None:
       """Foos a bar."""

**CML-BP-PYTHON-2**: Use type annotations
+++++++++++++++++++++++++++++++++++++++++

**Rationale**: Type annotations help address one of the biggest shortcomings of dynamically typed languages, where the
types allowed as inputs or expected as outputs from functions are not always easy to deduce.

**Example**

.. code-block:: python

   def connect(port: int, hostname: str) -> StatusCode:
       """Attempt to connect to a port on a host."""
       return implementation(port, hostname)

**CML-BP-PYTHON-3**: Functions should always return the same number of items, preferably only one item
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Functions which return a variable number of arguments depending on the logic path taken are brittle and
require users to understand the inner workings of the function which they are accessing.

**CML-BP-PYTHON-4**: Do not use classes in code intended for use in Trick input files
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

**Rationale**: Classes are designed to hold state and provide access control to data, while Trick input files are
designed to be a purely procedural interface to a simulation. Frequent overuse of classes in Trick input files has
proven repeatedly to negatively impact code usability and maintainability. Use free functions instead.

Coding Standards Waiver
-----------------------

Waivers to any coding standard may be granted on a case-by-case basis when it can be demonstrated that the following
criteria are both met:
1. Safeguards are in place to ensure that the exception does not create a risk to the overall project.
2. The effort required to bring the code to a satisfactory standard would be excessive.

The request for a waiver is made during Pull Request Review. The request may be in the form of a PowerPoint
presentation, a memo, or any other format capable of conveying the following information:
-  Violation (with code)
-  Options for mitigation/resolution
-  Rationale why the violation is acceptable
-  Evaluation of risk to accepting the waiver

If the waiver is granted, the request must be updated to reflect the approver, date of approval, and this information
must be recorded with the model documentation. The complete list of waivers will be maintained separately for CML
maintenance and metrics purposes. If the waiver is denied, the developer is directed to resolve the violation.

Appendix A: Acronyms and Abbreviations
======================================

.. list-table::
   :widths: 30 70

   * - Acronym
     - Abbreviation
   * - CML
     - Common Model Library
   * - CUI
     - Controlled Unclassified Information
   * - EAR
     - Export Administration Regulations
   * - ITAR
     - International Traffic in Arms Regulations
   * - JEOD
     - Johnson Space Center Engineering Orbital Dynamics
   * - JSC
     - Johnson Space Center

Appendix B: Glossary of Terms
=============================

.. list-table::
   :widths: 30 70

   * - Term
     - Description
   * - Trick
     - An open-source C++/Python driven simulation development framework. See https://github.com/nasa/trick for more
       information.
   * - Pull Request Review
     - When code is ready to be reviewed and accepted into the main production branch of CML, a pull request is opened.
       The Pull Request Review is the process by which the code is assessed for compliance with CML's code standards and
       other NASA standards by the CML team.
