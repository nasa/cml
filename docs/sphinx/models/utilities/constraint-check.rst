################
Constraint Check
################

.. list-table:: Revision History
   :widths: 15 30 30 50
   :header-rows: 1

   * - Version
     - Date
     - Author
     - Purpose
   * - 1
     - March 2024
     - Gary Turner
     - Initial version
   * - 2
     - October 2026
     - Nino Tarantino
     - Convert to reStructuredText and clean up model

________________________________________________________

Introduction
============

The constraint check model provides a generic capability to test the values of specified variables
against threshold values to evaluate whether constraints have been violated.

The model can be configured to support various levels of complexity. For example:

- A constraint requires that the variable, :math:`x`, not exceed some ceiling, :math:`y`; the constraint is
  violated immediately upon encountering a value :math:`x > y`
- A constraint requires that the value :math:`x` not be within a bounded box; the constraint is violated
  immediately upon encountering a value :math:`y_{l} < x < y_{u}`, where :math:`y_{l}`, :math:`y_{u}` are
  the lower and upper bounds, respectively.
- A constraint requires that the value :math:`x` not be larger than :math:`y` for more than :math:`t` seconds; for
  this type of constraint, the model enters an intermediate phase when :math:`x > y` and starts a timer. If
  the timer reaches :math:`t` before :math:`x \le y`, the constraint is marked as violated.
- A constraint requires that the value *x* not be smaller than some set of values :math:`y_{1}`, :math:`y_{2}`,
  :math:`y_{3}` for more time than allowed, with each :math:`y_{i}` value having a corresponding duration
  :math:`t_{i}`. The constraint is violated if
  :math:`{({{x < y_{1}}\mspace{9mu}\mathit{for}\mspace{9mu}\Delta{t > t_{1}}})} \vee {({{x < y_{2}}\mspace{9mu}\mathit{for}\mspace{9mu}\Delta{t > t_{2}}})} \vee {({{x < y_{3}}\mspace{9mu}\mathit{for}\mspace{9mu}\Delta{t > t_{3}}})}`.


Requirements
============

Nomenclature and Concepts
-------------------------

Requirements
------------

This model currently has no requirements and therefore has not been verified. Use at your own risk.


Model Specifications
====================

Architectural Considerations
----------------------------

Existing External Capabilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Inheritance
^^^^^^^^^^^

The top-level base-class, :cpp:class:`ConstraintSet`, inherits from the :cpp:class:`SubscriptionBase` model to handle
activity within a linked set of constraints.

Support
^^^^^^^

The model does not utilize other CML models in a supporting role.

Model Structure
---------------

Conceptually, the required capabilities can be divided into three categories: :ref:`constraint-test <constraint-test>`,
:ref:`constraint <constraint>`, and :ref:`constraint-set <constraint-set>`.

.. _constraint-test:

Constraint-Test
~~~~~~~~~~~~~~~

A constraint-test provides the mechanism for testing some value against some specified threshold
using some specified logical comparison. e.g. :math:`x < y`, :math:`x \ge y`, :math:`x \neq y`.
Constraint-tests are combined together to form a constraint; a constraint is conceptually violated
when its constraint-tests are violated. There are two underlying concepts to constraint-tests:

1. An instant-violation constraint-test (:cpp:class:`ConstraintTest`) has a simple Boolean :cpp:member:`~ConstraintTest::violation`
   flag that is set according to whether the variable meets the required test.
2. A timed-violation constraint-test (:cpp:class:`ConstraintTestTimed`) extends the simpler instant-violation
   constraint-test, adding support for monitoring the duration over which the constraint-test has
   been returning success and failure. This type of test is marked as violated only when the
   numerical test has been consistently failing for a specified period of time.

________________________________________________________

.. dropdown:: Constraint Test Base Classes

   .. doxygenfile:: constraint_test.hh

________________________________________________________

.. doxygenfile:: constraint_enum.hh

________________________________________________________

The two classes :cpp:class:`ConstraintTest` and :cpp:class:`ConstraintTestTimed` are then inherited into the following
class-templates, which provide the actual constraint-tests. The class-templates use the data-type of
the variable as the template parameter.

**Instantaneous constraint-tests**:

1. :cpp:class:`ConstraintTest_Threshold` compares the value of the variable against a single threshold value;
   this template does not have timer support.
2. :cpp:class:`ConstraintTest_Interval` checks for inclusion of the variable within (or without) an interval
   specified by an upper and lower bound; this template does not have timer support.
3. :cpp:class:`ConstraintTest_ValSet` checks for the variable matching (or not-matching) with one of a
   discrete set of values; this template does not have timer support.

**Timed constraint-tests**:

1. :cpp:class:`ConstraintTest_ThresholdTimed` compares the value of the variable against a single
   threshold value; this template does have timer support.
2. :cpp:class:`ConstraintTest_IntervalTimed` checks for inclusion of the variable within (or without) an
   interval specified by an upper and lower bound; this template does have timer support.
3. :cpp:class:`ConstraintTest_ValSetTimed` checks for the variable matching (or not-matching) with one of a
   discrete set of values; this template does have timer support.

________________________________________________________

.. dropdown:: Instantaneous Constraint Test Classes

   .. doxygenfile:: constraint_test_templates.hh

________________________________________________________

.. dropdown:: Timed Constraint Test Classes

   .. doxygenfile:: constraint_test_timed_templates.hh

________________________________________________________

.. _constraint:

Constraint
~~~~~~~~~~

A constraint comprises some number (at least 1) of constraint-tests. The :cpp:class:`Constraint` class is an
abstract class, providing a common foundation on which the specifically-configured instantiable
constraints are defined. The instantiable constraints are all represented as class-templates, taking
two template parameters:

7. the data-type of the variable being evaluated
8. the number of constraint-tests being made by this constraint

________________________________________________________

.. doxygenfile:: constraint.hh

________________________________________________________

A constraint may utilize any number of any combination of the constraint-tests outlined above. There
are seven pre-packaged class-templates for easy use, as follows:

1. :cpp:class:`ThresholdInstantConstraint` uses the specified number of :cpp:class:`ConstraintTest_Threshold` tests.
2. :cpp:class:`IntervalInstantConstraint` uses the specified number of :cpp:class:`ConstraintTest_Interval` tests.
3. :cpp:class:`ValSetInstantConstraint` uses the specified number of :cpp:class:`ConstraintTest_ValSet` tests.
4. :cpp:class:`ThresholdTimedConstraint` uses the specified number of :cpp:class:`ConstraintTest_ThresholdTimed` tests.
5. :cpp:class:`IntervalTimedConstraint` uses the specified number of :cpp:class:`ConstraintTest_IntervalTimed` tests.
6. :cpp:class:`ValSetTimedConstraint` uses the specified number of :cpp:class:`ConstraintTest_ValSetTimed` tests.
7. :cpp:class:`ThresholdTimedConstraintSpecData` provides an example of utilizing specified thresholds to
   define tests internally. This template takes three parameters:

   a. the data-type of the variable being evaluated
   b. the number of threshold values (and their respective violation times) per specification
   c. the number of threshold values (and their respective violation times) to test.

      An instance of this type will use the specified thresholds as interpolation anchor points, and
      compute the desired number of threshold-testing values to be equally spaced among those anchor
      values. The durations associated with each of these tests are then also interpolated from the
      anchor-durations; linear and logarithmic interpolation algorithms are included for computing
      the duration values.

________________________________________________________

.. dropdown:: Threshold Constraint Classes

   .. doxygenfile:: threshold_constraint.hh

________________________________________________________

.. dropdown:: Interval Constraint Classes

   .. doxygenfile:: interval_constraint.hh

________________________________________________________

.. dropdown:: Value-Set Constraint Classes

   .. doxygenfile:: valset_constraint.hh

________________________________________________________

.. _constraint-set:

Constraint-Set
~~~~~~~~~~~~~~

A constraint-set provides a loosely-associated list of constraints. This is a type of
constraint-manager, allowing constraints to be grouped so that they may be activated or deactivated
together. A constraint-set cannot be “violated”, such a concept is meaningless in its manager role.

________________________________________________________

.. doxygenfile:: constraint_set.hh

________________________________________________________

Example
~~~~~~~

Consider a situation in which a condition should be flagged if any of a set of tests fails to pass: for example:
((:math:`x > 0` for more than 3 seconds) or (:math:`x > 2` for more than 1 second)).

- Each of those tests will be represented as a :cpp:class:`ConstraintTest`, specifically as a
  :cpp:class:`ConstraintTest_ThresholdTimed` because both tests evaluate a variable against a threshold-value,
  with an associated timing constraint.

- While each of those tests could be represented as a :cpp:class:`Constraint` containing one :cpp:class:`ConstraintTest`,
  doing so would not be optimal in this configuration.

  - Because the condition should be flagged when either test fails, if each test is represented as a
    separate Constraint, some external model would have to compile the results of the two
    constraints and set the overall test result.
  - Because both tests monitor the same variable, both tests can be included in one constraint,
    which can then be configured to be marked as violated when any of its constraint-tests are
    violated.

  The optimal configuration for this scenario is to have one constraint, including two constraint
  tests – in this case an instance of :cpp:class:`ThresholdTimedConstraint`.

- Even though this example includes a set of evaluative tests, the overall configuration CANNOT be a
  :cpp:class:`ConstraintSet` because a :cpp:class:`ConstraintSet` has no concept of pass/fail. The :cpp:class:`ConstraintSet` concept
  is to manage the simultaneous evaluation of multiple constraints. The constraint (comprising
  multiple constraint-tests) may be managed by a constraint-set, but it cannot be conceptually
  represented by one.

Mathematical Formulation
------------------------

Interpolation
~~~~~~~~~~~~~

The :cpp:class:`ThresholdTimedConstraintSpecData` class-template includes the necessary to code to determine
the threshold values for testing from a set of threshold-values per specification.

For the purposes of illustration, suppose the specification threshold and associated time-limits
are:

========= ==========
threshold time-limit
========= ==========
1         100
5         10
10        1
========= ==========

If the constraint is configured to use 1 test, the model will always take the first set of values as
that test:

========= ==========
threshold time-limit
========= ==========
1         100
========= ==========

If the constraint is configured to use 2 tests, the model will always take the first and last set of
values as that test:

========= ==========
threshold time-limit
========= ==========
1         100
10        1
========= ==========

If the constraint is configured to use any other number of tests, the model will interpolate the
threshold values between the first and last set of values as that test: For example, if it was
configured for 3 tests:

========= ==========
threshold time-limit
========= ==========
1         100
5.5       \*
10        1
========= ==========

The value for the time-limit for an interpolated threshold is interpolated between the time-limits
of the adjacent specification values – in this case, 5.5 is between 5 and 10, so the time limit will
be between 10 and 1 respectively. This interpolation can be conducted using linear or logarithmic
interpolation.

Linear Interpolation
^^^^^^^^^^^^^^^^^^^^

Using subscripts *0* and *1* to represent the adjacent values from the specified data, we have for
linear interpolation:

.. math::
    :label: linear-interpolation-formulation
    :nowrap:

    \begin{equation}
        \begin{aligned}
           t &= t_{0} + f(t_{1} - t_{0}) \\
             &= t_{0}(1 - f) + t_{1} f
        \end{aligned}
    \end{equation}

where

.. math::
    :label: first-order-difference
    :nowrap:

    \begin{equation}
        f = \frac{x - x_{0}}{x_{1} - x_{0}}
    \end{equation}

In the illustration example, this results in the time-limit associated with threshold of *5.5*
being:

.. math::
    :label: linear-interpolation-example
    :nowrap:

    \begin{equation}
        \begin{aligned}
            t &= 10 + \left( \frac{5.5 - 5}{10 - 5} \right) (1 - 10) \\
              &= 10 - 0.9 \\
              &= 9.1
        \end{aligned}
    \end{equation}

========= ==========
threshold time-limit
========= ==========
1         100
5.5       9.1
10        1
========= ==========

Logarithmic Interpolation
^^^^^^^^^^^^^^^^^^^^^^^^^

Using subscripts *0* and *1* to represent the adjacent values from the specified data, we have for
logarithmic interpolation:

.. math::
    :label: log-interpolation-formulation
    :nowrap:

    \begin{equation}
        log(t) = log(t_0) + f(log(t_1) - log(t_0))
    \end{equation}

where

.. math::
    :label: log-difference
    :nowrap:

    \begin{equation}
        f = \frac{log(x) - log(x_0)}{log(x_1) - log(x_0)} = \frac{log\left(\frac{x}{x_0}\right)} {log\left(\frac{x_1}{x_0}\right)}
    \end{equation}

Note – the code arbitrarily uses base-10 logarithm; due to the nature of logarithms, the
selection of the base is irrelevant. Additionally, this set of calculations is only performed
once at model initialization; if there is a performance difference it is going to be negligible
and not worth the effort to evaluate.

i.e.

.. math::
    :label: log-time
    :nowrap:

    \begin{equation}
        log(t) = log(t_0) + f \times log\left(\frac{t_1}{t_0}\right) \Rightarrow t = t_0 \left( \frac{t_1}{t_0} \right)^f = t_0^{(1-f)} t_1^f
    \end{equation}

In the illustration example, this results in the fraction :math:`f` associated with threshold of *5.5*
being:

.. math::
    :label: log-threshold-example
    :nowrap:

    \begin{equation}
        f = \frac{log\left( \frac{5.5}{5} \right)} {log\left( \frac{10}{5} \right)} \approx 0.138
    \end{equation}

and the corresponding time-limit:

.. math::
    :label: log-time-limit-example
    :nowrap:

    \begin{equation}
        t = 10 \left(\frac{1}{10}\right)^{0.138} \approx 7.3
    \end{equation}

========= ==========
threshold time-limit
========= ==========
1         100
5.5       7.3
10        1
========= ==========

User’s Guide
============

Implementation
--------------

This section has not been filled out yet.

Initialization
--------------

This section has not been filled out yet.

Routine Execution
-----------------

This section has not been filled out yet.

Optional Execution
------------------

This section has not been filled out yet.

Configuration
-------------

ThresholdTimedConstraintSpecData
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The :cpp:class:`ThresholdTimedConstraintSpecData` has two important configuration settings not used by other
constraints:

- :cpp:member:`~ThresholdTimedConstraintSpecData::use_linear_interpolation`
- :cpp:member:`~ThresholdTimedConstraintSpecData::num_specs`

The :cpp:member:`~ThresholdTimedConstraintSpecData::use_linear_interpolation` setting switches the computation of the time-limit between using linear
and logarithmic interpolation between the specified data points.

The :cpp:member:`~ThresholdTimedConstraintSpecData::num_specs` variable is constructed to be equal to the number of specifications identified in
the template parameter. However, particularly for cases where the setting of those specification
values is not known at the time of construction, it may be necessary to adjust the number of
specifications post-construction.

For example, suppose a constraint may be populated from one of two specification data-sets, one of
which contains 3 specific data points, and the other contains 4. The instance must be constructed to
be capable of holding 4 in case the latter is used, but must be configurable to hold only 3 in case
the former is used.

The :cpp:func:`~ThresholdTimedConstraintSpecData::set_num_specs` method adjusts the number of specification values to actually use. It
cannot be adjusted to more than the instance was constructed to hold because the array has already
been sized accordingly. It cannot be adjusted after initialization because the interpolation is
conducted at initialization so there is no point in adjusting the number of data points to use for
interpolation after interpolation has completed.

Verification
============

Code Coverage
-------------

Exceptions
----------

Simulation Configurations
-------------------------

Unit-Test Cases
---------------

Dynamic-Test Cases
------------------
