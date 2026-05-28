/*******************************TRICK HEADER******************************
PURPOSE: (Generic vent model with mass depletion)

LIBRARY DEPENDENCY:
  (../src/vent.cc)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version)))
************************************************************************/

#ifndef CML_VENT_WITH_DYN_MASS_BODY_HH
#define CML_VENT_WITH_DYN_MASS_BODY_HH

#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "cml/models/dynamics/mass/dynamic_mass/include/dynamic_mass_body.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "simple_vent.hh"

class Vent : public SimpleVent {
  friend class VentSet;
 protected:
  DynamicMassBody& tank; /* (--) Tank from which mass is depleted */

  double flowrate;       /* (kg/s) Default flowrate */
  double exhaust_speed;  /* (m/s) Speed of the exhaust plume; this is also the
                                  N*s of impulse imparted per kg vented */
  double prev_time;      /* (s) Used for calculating delta time */
  bool flowrate_set;     /* (--) Internal flag indicating that this vent has
              knowledge of the rate at which mass should be depeleted */
  bool exhaust_set;      /* (--) Internal flag indicating that this vent has
              knowledge of the speed at which mass is expelled */


 public:
  Vent(const double& dyn_time,
       DynamicMassBody& tank_);
  virtual ~Vent() {};

  virtual void use_impulse_mode(bool mode = true) override;
  void set_force_magnitude(double force_, bool hold_impulse = false) override;
  void set_force_vector(double force_[3], bool hold_impulse = false) override;
  void set_flowrate( double flowrate,
                     bool hold_exhaust = true) override;
  void set_exhaust_speed( double exhaust_speed,
                          bool   hold_flowrate = true) override;

  double get_flowrate() override { return flowrate; }
  double get_exhaust_speed() override { return exhaust_speed; }
  bool flowrate_is_set() override { return flowrate_set; }
  bool exhaust_speed_is_set() override { return exhaust_set; }

  void unset_exhaust_speed() {exhaust_set = false;}
  void unset_flowrate() {flowrate_set = false;}

 protected:
  // Polymorphic override of SimpleVent implementation
  void set_force_internal() override;
  // Methods restricted so as to limit access to VentSet only
  void update() override;
  bool start_venting() override;
  void check_status() override;

  // Methods restricted because they are intended to be internal methods
  bool check_configuration() override;
  void set_exhaust_flowrate_from_force(bool hold_exhaust = true);
  void update_mass_demand() override;
  void stop_venting() override;

 private:
  Vent(const Vent&);
  Vent& operator= (const Vent&);
};
#endif