/*******************************TRICK HEADER******************************
PURPOSE: (Class for manipulating sets of vents and aggregating forces and
          moments)

LIBRARY DEPENDENCY:
  (../src/vent_set.cc)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (initial version)))
 ************************************************************************/

#ifndef CML_VENT_SET_HH
#define CML_VENT_SET_HH

#include <vector> // vector
#include <list>
#include "jeod/models/utils/math/include/vector3.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "simple_vent.hh"
#include "vent.hh"

class VentSet : public SubscriptionBase {
 protected: // external references
  jeod::DynBody& dyn_body; /* (--) jeod::DynBody to which vents are attached */
  const double& dyn_time; /* (s) Dynamic time for timed vents */

 public:
  bool start_vents_at_activation; /* (--)
       Flag indicates whether "activation" means "start-venting".
       Because the model inherits from SubscriptionBase, it will be activated
       with its first subscription; this flag controls whether that means
       venting should start with the first subscription.
       Typically set to true if all vents are in dynamic-mode and false if
       some are in impulse-mode.  Default: False. */
  double force[3];               /* (N)
       The 3-vector force (struc frm) produced by all vents */
  double torque[3];              /* (N*m)
       The 3-vector torque (struc frm) produced by all vents */

 protected:
  std::vector< SimpleVent*> vents;  /* (--)
     Pointers to the vents in this set. */
  std::list< SimpleVent*> dynamic_vents;  /* (--)
     Pointers to the vents currently venting and producing a force. */
  std::list< SimpleVent*> impulsive_vents;  /* (--)
     Pointers to the vents currently set to apply an impulse. */

 private:
  double ang_impulse[3]; /* (N*m*s)
     The angular impulse applied to the body; used in the case that vents
     are configured to apply impulsively.  Serves as a working variable; when
     passed to apply_impulse_to_body, represents the angular-impulse expressed
     in the root-body's body-frame.*/

 public:
  // This default constructor initializes all the members of the set except the
  // vents array. It is called by all the other constructors, which eliminates
  // duplication of the initialization list.
  VentSet(jeod::DynBody & dyn_body_,
          const double & time);
  VentSet(size_t num_vents,
          jeod::DynBody & dyn_body_,
          const double & time);
  VentSet(size_t num_vents,
          jeod::DynBody & dyn_body_,
          const double & time,
          DynamicMassBody & tank);
#ifndef SWIG
  // Hide this one from SWIG because SWIG cannot distinguish between
  // "DynamicMassBody &" and "DynamicMassBody *"
  VentSet(size_t num_vents,
          jeod::DynBody & dyn_body_,
          const double & time,
          DynamicMassBody * tank_array);
#endif
  // TODO Turner 2020/01
  //  - implement a Vent type running off a DynamicMassString rather than a
  //    single tank.
  //  - investigate use of varargs or variadic templates to support a set
  //    of vents each running off its own specified DynMassBody.
  virtual ~VentSet();

  virtual void initialize();
  virtual void update();
  virtual void start_vent(unsigned int);
  virtual void start_vents();
  virtual void stop_vent(unsigned int);
  virtual void stop_vents();
  void use_impulse_mode(bool mode = true);
  void use_dynamic_mode(bool mode = true) { use_impulse_mode(!mode); }
  void add_vent( SimpleVent & vent) { vents.push_back(&vent); }
  SimpleVent * get_vent(size_t ii);
 protected:
  void collect_force_torque();
  void apply_impulse();
  void apply_impulse_to_body( jeod::DynBody &);
  virtual void activate();
  virtual void deactivate();

 private:
  void start_vent_internal( SimpleVent * vent);

 private:
  VentSet(const VentSet&);
  VentSet& operator = (const VentSet&);
};
#endif
