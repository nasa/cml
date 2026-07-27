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

#ifndef CML_SUBSCRIPTIONS_HH
#define CML_SUBSCRIPTIONS_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include <string>

/**
 * Provides a subscription baseline for all models
 *
 * Models can subscribe to anything that inherits from this class. When the
 * first subscription is processed, the model is activated. When subsequent
 * subscriptions are processed, nothing happens. When the last subscription
 * is canceled, the model deactivates.
 */
class SubscriptionBase {

 public:
  std::string subscribe_name {"unnamed-instance"}; /**< (--)
      Optional setting, useful for messages and debugging only.
      Not used for other purposes.*/
  bool initialize_on_failed_activation {false}; /**< (--)
      Configuration flag controls the response if the model fails to activate
      during an initialization operation with subscriptions already pending.
      If false, retain subscriptions but fail initialization. */
  bool quiet_unsubscribe_warning {false}; /**< (--)
      Optional flag to control whether a warning is printed when attempting to
      unsubscribe from a model with no active or pending subscriptions. */
  bool quiet_disabled_warning {false}; /**< (--)
      Optional flag to quiet the error meesage that would be posted when
      subscribing to a disabled model.*/

 protected:
  bool enabled {true}; /**< (--)
      Master flag. Use as a gate on #initialize() if desired. If false, will
      prevent subscription requests. */
  bool initialized {false}; /**< (--)
      The model is ready to be activated */
  bool active {false}; /**< (--)
      The model can be executed. The first line of code in the model inheriting
      from this should be
      \code{.cpp}
      if (!active) {
          return;
      }
      \endcode */
  int  sub_pending {0}; /**< (--)
      Internal subscription count. Incremented if subscriptions are received
      prior to initialization. */
  int  num_subscriptions {0}; /**< (--)
      Number of subscriptions, post-initialization. */

 public:
  /**
   * Default constructor
   */
  SubscriptionBase() = default;
  /**
   * Copy constructor deleted
   */
  SubscriptionBase (const SubscriptionBase&) = delete;
  /**
   * Copy assignment operator deleted
   */
  SubscriptionBase& operator = (const SubscriptionBase&) = delete;

  /**
   * Virtual destructor
   */
  virtual ~SubscriptionBase() = default;

  /**
   * Instructs this model to turn itself on if everything is configured correctly
   *
   * This may be called before initialization, in which case it is unlikely that
   * everything is configured correctly. In that case, the #sub_pending flag gets
   * set so that when initialize gets called, the subscription can be completed.
   */
  virtual void subscribe();

  /**
   * Tells this model that whoever had previously subscribed to it no longer needs it
   *
   * If this model had only one subscription, the last customer just left the
   * building and this model can turn itself off.
   *
   * @note Takes no action if the model is already disabled.
   */
  virtual void unsubscribe();

  /**
   * Configure pending subscriptions
   *
   * If this class is inherited from, calling this method should be the last step
   * in a child class' initialize() method.
   *
   * @warning Calling this function will internally mark the object as initialized.
   *          If your child class has input parameters which need to be checked
   *          for correctness before marking itself as initialized, make sure to
   *          do so before calling this function.
   */
  virtual void initialize();

  /**
   * Completely disables the model
   *
   * Sets #active to `false`, preventing the execution of the model, regardless of
   * the current number of active or pending subscriptions.
   *
   * To re-enable the model requires setting the #enabled flag and re-subscribing.
   */
  virtual void disable();

  /**
   * @return `true` if the model has successfully been initialized
   */
  bool is_initialized() const { return initialized;}

  /**
   * @return `true` if the model is #active
   */
  bool is_active() const { return active;}

  /**
   * @return `true` if the model is #enabled
   */
  bool is_enabled() const {return enabled;}

 protected:
  /**
   * Internal subscription logic
   */
  void subscribe_internal();

  /**
   * Enable execution of the model
   *
   * @note Derived classes will typically override this to include a call to the
   *       specific update() implementation and potentially subscribe to lower-
   *       level components within the class.
   */
  virtual void activate(){ active = true;}

  /**
   * Disable execution of the model
   *
   * @note Derived classes should remember to unsubscribe from any subscriptions
   *       made during #activate().
   */
  virtual void deactivate(){active = false;}
};
#endif
