/*******************************TRICK HEADER******************************
 PURPOSE: ( Provide the subscription methods)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (3/2015) (Antares) (initial version)))
***********************************************************************/

#include "../include/subscriptions.hh"

/*****************************************************************************
subscribe
Purpose:(Public method.  Instructs this model to turn itself on if everything
         is configured correctly.
         Subscribe may be called before initialization, in which case it is
         unlikely that everything is configured correctly.  In that case, the
         sub_pending flag gets set so that when initialize gets called, the
         subscription can be completed.)
*****************************************************************************/
void
SubscriptionBase::subscribe()
{
  if (!enabled) {
    if (!quiet_disabled_warning) {
      CMLMessage::error(
       __FILE__, __LINE__, "Subscription Error\n",
       "Model (", subscribe_name, ") has been disabled for this scenario. \n"
       "Cannot subscribe to a disabled model.\n");
    }
    quiet_disabled_warning = false; // this is a single-use flag.
  }
  // enabled
  else if (initialized) {
    subscribe_internal();
  }
  // enabled but not initialized.  Leave the subscription pending
  // completion of the initialization.
  else {
    sub_pending ++;
  }
}

/*****************************************************************************
unsubscribe
Purpose:
  Public method.  Tells this model that whoever had previously
  subscribed to it no longer needs it.  If this model had only one
  subscription, the last customer just left the building and this model
  can turn itself off.
Note:
  It is acceptable to silently take no action while disabled because failing
  to turn off a model that is already turned off does not have significant
  executable consequence.
  Taking no action while disabled prevents getting inadvertent warnings about
  trying to unsubscribe from a non-subscribed model if subscriptions were
  either circumvented or blocked as a result of being disabled.
  Note that this really only applies to top-level instances -- i.e. those
  whose activity are managed at the sim-level rather than by another model.
  If some model is trying to unsubscribe one of its dependencies, and that
  dependency has already been disabled, then something is likely already wrong
  with the model.
*****************************************************************************/
void
SubscriptionBase::unsubscribe()
{
  if (!enabled) {
    return;
  }

  // Support pre-initialization unsubscribes.  This is important, for example,
  // where a model is subscribed in default-data, but turned off in the
  // pre-init configuration of a particular scenario.
  if (!initialized) {
    if (sub_pending > 0) {
      sub_pending--;
    }
    else if (!quiet_unsubscribe_warning) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Pre-init unsubscribe error.\n",
        "Instruction received to unsubscribe the model (", subscribe_name, ") but there are no\n"
        "pending subscriptions.  Check your configuration.\n"
        "Cannot process unsubscriptions in anticipation of incoming\n"
        "subscriptions.\nCommand failed.\nModel remains unsubscribed.\n");
    }
    // else, silently take no action.
  }
  else { // post-initialization
    if (num_subscriptions > 0) {
      num_subscriptions--;
    }
    else if (!quiet_unsubscribe_warning) {
      CMLMessage::warn(
        __FILE__,__LINE__,"Post-init unsubscribe error.\n",
        "Instruction received to unsubscribe the model (", subscribe_name, ") but there are no\n"
        "active subscriptions.  Check your configuration.\n"
        "Cannot process unsubscriptions in anticipation of incoming\n"
        "subscriptions.\nCommand failed.\nModel remains unsubscribed.\n");
    }
    // else, silently take no action.
    if (num_subscriptions <= 0 && active) {
      CMLMessage::inform(
        __FILE__,__LINE__,"Deactivation from Unsubscribe.\n",
        "Instruction received to remove the last subscription to model (", subscribe_name, ").\n"
        "Consequentially deactivating model.\n");
      deactivate();
    }
  }
}

/*****************************************************************************
initialize
Purpose:(Public method, intended to be re-written for specific implementation
         If this model is re-written, the last step in the derived class
         should be to call this implementation.
         NOTE - DO NOT CALL THIS BEFORE COMPLETION OF TESTING OF ALL
                PARAMETERS IN THE TERMINAL (i.e. derived) CLASS.
                THIS METHOD WILL SET THE initialized FLAG TO true.
         If this is called after subscribe(), the subscribe_internal() method
         will be called to complete the subscription process.)
*****************************************************************************/
void
SubscriptionBase::initialize()
{
  if (!enabled) { return; }
  initialized = true;
  if (sub_pending > 0) {
    subscribe_internal();
    // if activation -- resulting from having subscriptions pending -- was
    // successful, be sure to count all of the intended subscriptions;
    // subscribe_internal will only count 1.
    if (active) {
      num_subscriptions += (sub_pending -1); // added one automatically.
      sub_pending = 0;
    }
    // If activation failed, then we have a choice to make:
    // - mark the model as initialized and fail to activate it, removing all
    //   pending subscriptions.
    // - fail the initialization instead and retain the pending subscriptions.
    // The latter option is more user-friendly because it allows for the later
    // initialization of the model without having to recover the source of
    // the failed pending subscriptions; this is default.
    else if (initialize_on_failed_activation) {
      CMLMessage::error(
        __FILE__,__LINE__,"Failure During Initialization.\n",
        "The SubscriptionBase initialization for '", subscribe_name, "' failed when the model\n"
        "attempted to activate during initialization:\n"
        " - activation sequence executed due to having pending subscriptions.\n"
        "Model is marked as having been initialized but not activated;\n"
        "pending-subscriptions have been removed per setting of\n"
        "configuration flag initialize_on_failed_activation.\n"
        "Re-subscribe to the model to activate it.\n");
      sub_pending = 0;
      num_subscriptions = 0;
    }
    else {
      CMLMessage::error(
        __FILE__,__LINE__,"Failure During Initialization.\n",
        "The SubscriptionBase initialization for '", subscribe_name, "' failed when the model\n"
        "attempted to activate during initialization:\n"
        " - activation sequence executed due to having pending subscriptions.\n"
        "Model has been neither initialized nor activated\n"
        "but pending subscriptions have been retained per setting of\n"
        "configuration flag initialize_on_failed_activation.\n"
        "Rerun <model>.initialize() to apply pending subscriptions and "
        "activate the model.\n");
      initialized = false;
      num_subscriptions = 0;
    }
  }
}

/*****************************************************************************
disable
Purpose:(Completely disables the model.  To re-enable requires setting the
         enabled flag and re-subscribing.
         Intention is that this is a virtual method.)
*****************************************************************************/
void
SubscriptionBase::disable()
{
  deactivate();
  enabled = false;
}

/*****************************************************************************
subscribe_internal
Purpose:(Protected method, called from subscribe or initialize, whichever is
         called second)
*****************************************************************************/
void
SubscriptionBase::subscribe_internal()
{
  // Increment num_subscriptions first so that unsubscribe() can be called
  // safely from a failure in activate()
  num_subscriptions++;
  if (!active) {
    activate();
  }
  if (!active) {
    // It should be impossible to have a positive subscription count while the
    // model is inactive. Ideally, the derived class's activate() method should
    // get rid of the subscription if activation fails; but in case it doesn't,
    // any such subscriptions are removed here.
    num_subscriptions=0;
  }
}