/*******************************TRICK HEADER******************************
 PURPOSE: (Provides a subscription baseline for all models.
           Models can subscribe to anything that inherits from this class.
           When the first subscription is processed, the model is activated.
           When subsequent subscriptions are processed, nothing happens
           When the last subscription is canceled, the model deactivates)

 LIBRARY DEPENDENCY:
    ((../src/subscriptions.cc))

 PROGRAMMERS:
   (((Gary Turner) (OSR) (Mar 2015) (Antares) (initial model))
    ((Bingquan Wang) (OSR) (May 2017) (Antares)
                           (cleaned up the code per its IV&V code review))
   )
***********************************************************************/

#ifndef ANTARES_SUBSCRIPTION_BASE_HH
#define ANTARES_SUBSCRIPTION_BASE_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <string>

class SubscriptionBase {

 public:
  std::string  subscribe_name; /* (--)
      Optional setting, useful for messages and debugging only.
      Not used for other purposes.*/
  bool  initialize_on_failed_activation; /* (--)
      Configuration flag controls the response if the model fails to activate
      during an initialization operation with subscriptions already pending.
      Default: false (retain subscriptions, fail initialization).*/
  bool quiet_unsubscribe_warning; /* (--)
      Optional flag to quiet the unsubscribe warning.*/
  bool quiet_disabled_warning; /* (--)
      Optional flag to quiet the error meesage that would be posted when
      subscribing to a disabled model.*/

 protected:
  bool enabled; /* (--) master flag. Use as a gate on initialize() if desired. */
  bool initialized; /* (--) model is ready to be activated */
  bool active;      /* (--) model can be executed */
  int  sub_pending; /* (--) Subscription call received before initialization */
  int  num_subscriptions; /* (--) number of subscriptions. */

 public:
  SubscriptionBase() :
    subscribe_name("unnamed-instance"),
    initialize_on_failed_activation(false),
    quiet_unsubscribe_warning(false),
    quiet_disabled_warning(false),
    enabled(true),
    initialized(false),
    active(false),
    sub_pending(0),
    num_subscriptions(0){};

  virtual ~SubscriptionBase(){};

  virtual void subscribe();
  virtual void unsubscribe();
  virtual void initialize();
  virtual void disable();
  bool is_initialized() const { return initialized;};
  bool is_active() const { return active;};
  bool is_enabled() const {return enabled;};

 protected:
  void subscribe_internal();

  // Note - will typically include at least a call to update(),
  //        and often subscriptions to other models.
  virtual void activate(){ active = true;}
  // Note - remember to unsubscribe any subscriptions made during activate().
  virtual void deactivate(){active = false;}

 private: // and undefined:
  SubscriptionBase (const SubscriptionBase&);
  SubscriptionBase& operator = (const SubscriptionBase&);
};
#endif