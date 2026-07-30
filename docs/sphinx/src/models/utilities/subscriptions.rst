Subscriptions
+++++++++++++

.. list-table:: Revision History
   :widths: 30 30 30 70

   * - Version
     - Date
     - Author
     - Purpose
   * - 1
     - July 2015
     - Gary Turner
     - Initial version
   * - 2
     - March 2022
     - Gary Turner
     - Added verification and improved user's guide
   * - 3
     - December 2022
     - Gary Turner
     - Toggle for silently failing when unsubscribing a disabled model
   * - 4
     - September 2025
     - Hirad Mirhashemi
     - Updated verification and user's guide

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

Introduction
============

The Subscriptions Model is used as a base for many models; its primary
purpose is to facilitate the activation and deactivation of models as
they are needed at different phases of a complex simulation.

Consider a simulation that includes two models, A and B, with model A
requiring input from model B. For some part of the simulation, model A
is required, so its execution must be scheduled in the master simulation
sequence. Whenever model A runs, model B must also run, so its execution
must be scheduled in the master simulation sequence. But running both
models all of the time is highly wasteful, and will slow the simulation
unnecessarily. So, for the times that the models are not needed, their
scheduled execution should simple return without additional process.
Thus, we have the concept of models being active and inactive, and of
subscriptions that toggle between the two states.

At some point in the simulation, the simulation subscribes to model A,
flagging it as needing to run. In turn, model A subscribes to model B,
flagging it as needing to run. As long as a model is subscribed, its
scheduled execution will run. If it is unsubscribed, its scheduled
execution will return without process. Then, when model A is no longer
needed, the simulation can unsubscribe from it, and model A can
unsubscribe from model B, potentially deactivating both models and
preventing their further execution.

Here, I say potentially deactivating both models because it may not be
appropriate to stop them. Suppose a third model, C, also requires input
from model B and has been subscribed in the interim time. Stopping model
B with the deactivation of model A will break model C. So we need a
count of the number of targets depending on a model being executed. If
that count is zero, the model does not need to run.

This model handles that subscription count for all models that inherit
it.

.. _requirements-1:

Requirements
============

1. The model shall provide an active flag to inheriting models to
   identify whether those model should be run.
2. This model shall control the active flag via a count of the number of
   demands placed on the inheriting models.
3. This model shall provide the option to permanently deactivate a
   model, preventing further demands from re-activating it.

Model Specifications
====================

Architectural Considerations
----------------------------

Existing External Capabilities
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Inheritance
^^^^^^^^^^^

The :cpp:class:`SubscriptionBase` class that forms this model is a base class with
no inheritance.

Support
^^^^^^^

No dependencies.

Model Structure
~~~~~~~~~~~~~~~

.. doxygenclass:: SubscriptionBase
   :members:
   :protected-members:
   :undoc-members:

Mathematical Formulation
------------------------

No mathematical formulation.

User's Guide
============

This model is not intended to be used as a stand-alone model, but rather
as the base for other models that need to be activated or deactivated
during a complex simulation.

Notes on Disabling a Model
--------------------------

The design decision as to how disabling the model affects the
subscription-count and pending-subscription-count is complicated. There
are two possible results for each count - being that they are left
untouched, or set to zero. Identifying the best default behavior
requires consideration of how to process :cpp:func:`~SubscriptionBase::initialize`, :cpp:expr:`YourModel::update()`,
:cpp:func:`~SubscriptionBase::subscribe` and :cpp:func:`~SubscriptionBase::unsubscribe` calls while the model is disabled.

It is generally desirable that a disabled model cannot be initialized.
Calls to :cpp:func:`~SubscriptionBase::initialize` while disabled should be blocked. This is an
important features that supports blocking of expensive and
time-consuming initialization routines in models inheriting this
capability when said models are not necessary for a particular scenario.

Similarly, it is a required feature that calls to :cpp:expr:`YourModel::update()`
should not be processed while a model is *inactive*; this is the main
purpose of the model. The activity flag can be set by a call to
:cpp:func:`~SubscriptionBase::subscribe` if the model is initialized. Therefore, to support cases
where a model is disabled post-initialization, calls to :cpp:func:`~SubscriptionBase::subscribe`
must not set the active flag when the model is disabled, thereby
preventing a disabled model from executing. Any calls to :cpp:func:`~SubscriptionBase::subscribe()`
while the model is disabled **should be interpreted as user-error and
therefore flagged with a message**; a model should not be simultaneously
required (as suggested by the :cpp:func:`~SubscriptionBase::subscribe` call) and not available (as
suggested by being disabled).

Because of the inherent design associated with blocking :cpp:expr:`YourModel::update()` calls
for inactive models (previous paragraph), calls to :cpp:func:`~SubscriptionBase::subscribe` while
a model is disabled are immediately flagged as errors and no further
action is taken. This decision has implications on how to handle the
subscription-count and pending-subscription-count, discussed below.

By symmetry, calls to :cpp:func:`~SubscriptionBase::unsubscribe` while disabled should also
result in no further action being taken. This is less significant than
calls to :cpp:func:`~SubscriptionBase::subscribe` because the model is not executing anyway while
it is disabled; failing to turn it off while it is already off has no
executable consequence. However, when an :cpp:func:`~SubscriptionBase::unsubscribe` call is made,
the conceptual design calls for the subscriptions-count (or
pending-subscriptions-count) to be decremented. This count should never
go below zero because :cpp:func:`~SubscriptionBase::unsubscribe` calls should always follow
:cpp:func:`~SubscriptionBase::subscribe` calls. Internal sanity checking requires that an error
message be posted if the an :cpp:func:`~SubscriptionBase::unsubscribe` is received while the
respective count is equal to zero because that implies a
misconfiguration. This is where the logical paths start to conflict. If
commanding :cpp:func:`~SubscriptionBase::unsubscribe` while disabled results in no operation, then
subscriptions posted before the model was disabled could still be
included in the count. Conversely, if commanding :cpp:func:`~SubscriptionBase::unsubscribe` while
disabled results in decrementing the counts, then subscriptions posted
after the model was disabled (or circumvented in sim-configuration in
anticipation of the model being disabled) would result in a confusing
error message when attempting to decrement the respective counts from
zero. The latter option is the more objectionable outcome, so the design
decision follows the path of symmetry, that calls to :cpp:func:`~SubscriptionBase::unsubscribe` are
not processed (exiting silently) while the model is disabled.

However, this raises a problem of its own in the unlikely scenario that
a model is only temporarily disabled - a possibility that is not
supported in this core capability, but feasibly implementable in a
derived class. If a model is re-enabled, its count of subscriptions is
unreliable given that any :cpp:func:`~SubscriptionBase::subscribe` commands and :cpp:func:`~SubscriptionBase::unsubscribe`
commands have exited without affecting the count while the model was
disabled. This makes the value of the counts at this point largely
arbitrary. The two most obvious values would be zero, or the value the
model had at the time it was previously disabled. Potential use-cases
have been evaluated with neither resolution being universally desirable.
So we have a wholly arbitrary decision to make for a situation that is
not even supported by this model. **The decision was made to leave the
counts untouched at the time the model is disabled to better support
debugging**. This decision could be revisited at some time in the
future.

Control Flags
-------------

The model provides three optional control flags:

-  :cpp:member:`~SubscriptionBase::initialize_on_failed_activation` controls the consequences when
   the model is initialized with pending subscriptions and the
   consequential activation fails, while the initialization would
   otherwise have been successful without pending subscriptions. Note:

   -  Initializing with pending subscriptions will automatically lead to
      an attempt to activate the model. This is a deliberate design
      decision to make the activation of the model independent of the
      whether the subscription comes before or after the initialization
      of the model; both processes are necessary and sufficient for
      activation to be attempted.
   -  Note that while failing activation is not a plausible scenario in
      this base model (where the :cpp:func:`~SubscriptionBase::activate` method simply sets the
      :cpp:member:`~SubscriptionBase::active` flag to true), it is a very real possibility in a derived
      model where the :cpp:func:`~SubscriptionBase::activate` method might check availability or
      compatibility of some data set, or check for null pointers, or
      other pre-executive verification activities.
   -  Initialization and activation are independent processes with
      independent objectives. The situation of interest here is the one
      in which the model successfully initializes, but the activation
      step fails.

   A model that is both initialized and subscribed should be activated.
   If the activation fails, then one of the two pillars must also be
   failed. This flag identifies which to fail:

   -  :cpp:member:`initialize_on_failed_activation = false <SubscriptionBase::initialize_on_failed_activation>`
      results in
      setting the model’s *initialized* flag to false (indicating that
      initialization failed), but it retains its pending subscriptions.
   -  :cpp:member:`initialize_on_failed_activation = true <SubscriptionBase::initialize_on_failed_activation>` results in the model being
      marked as successfully initialized but the pending subscriptions
      are removed.

   Specific error messages are produced to alert the user to the outcome
   of the failed activation.

-  :cpp:member:`~SubscriptionBase::quiet_unsubscribe_warning` provides a means of
   suppressing warnings that would typically be generated when
   attempting to unsubscribe from a model that has no subscriptions.

   -  The underlying design assumption has :cpp:func:`~SubscriptionBase::unsubscribe` only being
      called from the same unit that previously generated the
      :cpp:func:`~SubscriptionBase::subscribe` call, so these calls should always occur in pairs
      and the situation in which :cpp:func:`~SubscriptionBase::unsubscribe` is called without a
      previous :cpp:func:`~SubscriptionBase::subscribe` should not arise. If an unsubscribed model
      receives instruction to remove a subscription, this is usually
      indicative of a problem in the model's architecture and a warning
      message is generated.

   -  This pairing of calls is especially important that when a model
      can be subscribed from multiple locations. Having one model remove
      another's subscription fundamentally breaks the purpose of the
      model.

   -  However, when a model has only one dedicated subscriber, this
      architecture can be onerous on the object making the
      :cpp:func:`~SubscriptionBase::subscribe`/:cpp:func:`~SubscriptionBase::unsubscribe` calls. There are going to be cases in
      which a subscription is conditional upon some configuration
      setting, and for which that setting is no longer testable at the
      point of decision over whether to call :cpp:func:`~SubscriptionBase::unsubscribe`. For these
      situations, it would be necessary to do one of:

      -  Add an internal flag, effectively confirming that the sub-model
         has been subscribed and that it can therefore be unsubscribed.
         This is necessary when the sub-model may be subscribed from
         multiple locations, but unnecessarily onerous for maintaining
         the activity of a dedicated sub-model.
      -  Check the active status of the sub-model to identify whether it
         has been subscribed. This is fundamentally risky and should
         **never** be used when the sub-model may be subscribed from
         multiple locations (for reasons that should be apparent); for
         maintaining the activity of a dedicated sub-model it is an
         acceptable option and less onerous than maintaining an internal
         flag, but still requires an additional logic step that may be
         difficult to inject in some situations.
      -  Issue :cpp:func:`~SubscriptionBase::unsubscribe` unconditionally and accept (and ignore)
         the warning message generated when the sub-model was not
         previously subscribed. Particularly for large projects, having
         an architecture in which some warning and error messages are
         “standard operating procedure” can get very difficult to
         manage, and easily leads to obfuscation of messages that should
         be addressed. This is bad practice and not a viable solution.
      -  Issue :cpp:func:`~SubscriptionBase::unsubscribe` unconditionally and bypass the generation
         of the warning message. This is fundamentally risky and should
         **never** be used when the sub-model may be subscribed from
         multiple locations (for reasons that should be apparent); for
         maintaining the activity of a dedicated sub-model it is an
         acceptable and simple option.

   The :cpp:member:`~SubscriptionBase::quiet_unsubscribe_warning` flag supports the last of these
   options by allowing the higher-level object to unconditionally
   unsubscribe from the sub-model. If the subscription had previously
   been applied, :cpp:func:`~SubscriptionBase::unsubscribe` removes it. If it had not been applied
   (and this flag is set to *true*), :cpp:func:`~SubscriptionBase::unsubscribe` has no effect.

   Note that the use of this flag is inherently dangerous and should
   only be used when there is no possibility that some other model may
   have subscribed to the sub-model in question.

-  :cpp:member:`~SubscriptionBase::quiet_disabled_warning` is similar in intent to the
   :cpp:member:`~SubscriptionBase::quiet_unsubscribe_warning` flag; in this case we consider the error
   message that is posted when a subscription is made to a model that
   has been disabled. This sequence is a more serious problem then that
   of unsubscribing from an inactive model:

   -  With :cpp:member:`~SubscriptionBase::quiet_unsubscribe_warning` the situation is that some
      entity is communicating that one of the models on which it depends
      is no longer required, but that dependency was not active anyway.
      The effect is typically negligible, the model was inactive before,
      and it probably should be inactive now. A warning is issued to
      alert the user to a possible misconfiguration that may have
      affected data upstream, but the downstream effect of not having a
      model available that isn't needed anyway is not significant.
      Blocking this warning message is quite reasonable.
   -  The :cpp:member:`~SubscriptionBase::quiet_disabled_warning` flag is more significant. With this
      flag, we are removing an error message that may otherwise be used
      to alert the user to significant downstream data effects. In this
      situation, some entity may be communicating that it requires
      support from some other model, but that model is not available to
      provide that support. The downstream data is likely to be affected
      by this configuration, and an error message is the appropriate
      response - all data beyond this point is suspect - and should be
      issued in this case.

   This flag is primarily included to support a situation in which the
   supporting model is not truly *required*, but simply useful or
   desirable in some circumstances. In this situation, there may be
   use-cases where the supporting model is desired, and use-cases where
   it is not. As an example, consider some simulation-event that
   triggers the activation of some model:

   -  that activated model isn't *required* by the event, but it is
      desirable that in at least some cases, the model be activated in
      response to the simulation-event;
   -  to support use-cases where the model is not needed, that model may
      be disabled but that would result in a subscription to a disabled
      model and consequential error message even though this was the
      intended pattern.

   Because blocking this message can have serious consequences, **this
   flag has been implemented as a single-use flag**. It can be set to
   true to block the error message from a specific :cpp:func:`~SubscriptionBase::subscribe` call,
   but **it resets to false** and any subsequent :cpp:func:`~SubscriptionBase::subscribe` calls
   will trigger the error message again (unless the flag is set back to
   true before each subsequent call).

Extension
---------

The following methods may need redefining in the derived class:

Initialize
~~~~~~~~~~

This new method should perform all initialization steps required of the
new model. The final step should be a call to
:cpp:func:`SubscriptionBase::initialize` where the initialized flag gets
set.

Disable
~~~~~~~

The base implementation deactivates and disables the model, leaving the
subscription counts untouched. This should be sufficient for most
applications.

Activate
~~~~~~~~

It may be desirable to include an automatic call to the model's regular
execution (e.g. :cpp:expr:`YourModel::update()`) as a part of the activation process. This is
not included in the base implementation.

It may also be necessary to subscribe to additional model dependencies.

The base model rejects subscriptions when activation fails in response
to a :cpp:func:`~SubscriptionBase::subscribe` call on a pre-initialized instance. In this case, the
model will remain inactive and the number of subscriptions will not be
incremented. Any :cpp:func:`~SubscriptionBase::unsubscribe` calls that come after the failed
activation will generate a warning (unless :cpp:member:`~SubscriptionBase::quiet_unsubscribe_warning`
is set to true).

Deactivate
~~~~~~~~~~

If the :cpp:func:`~SubscriptionBase::activate` call resulted in subscription to other models,
these should be unsubscribed at :cpp:func:`~SubscriptionBase::deactivate`.

