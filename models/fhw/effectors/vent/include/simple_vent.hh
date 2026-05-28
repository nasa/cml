/*******************************TRICK HEADER******************************
PURPOSE: (Simple vent force model - calculates force and torque from a vent
          but does not model mass depletion)

LIBRARY DEPENDENCY:
  ((../src/simple_vent.cc))

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (Antares) (Initial version))
   ((Tony Varesic) (OSR) (Nov 2020) (Antares) (added getters for:
     start_time, stop_time, venting and apply_as_impulse)))
 ************************************************************************/

#ifndef CML_SIMPLE_VENT_HH
#define CML_SIMPLE_VENT_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"

class SimpleVent {
  friend class VentSet;

 protected:
  const double& dyn_time; /* (s) Dynamic time for timed vents */

 public:
  double location[3]; /* (m)
     Position of vent in dyn_body's structural frame */
  bool active; /* (--)
      Flag controlling whether the vent is able to be switched on.
      Default: true */
  bool indefinite_duration; /* (--)
      Flag to control when vent closes.  If true, vent is opened with no
      specified duration and will remain open until stop_venting() called.
      Default: false. */
  bool quiet_message_start_inactive; /* (--)
      Flag used to identify whether the print an error message if a vent
      receives an instruction to start venting while inactive.
      This combination could be an error, or the system may deliberately use
      the active flag as a determination of whether to start venting.*/

 protected:
  bool initialized; /* (--)
      Internal flag indicating that the initialize method has completed
      successfully */
  bool apply_as_impulse; /* (--)
      Flag indicating that the vent is to operate with an impulsive behavior;
      when the vent is switched on it will provide a single instantaneous
      impulse only.  Default: false. */
  bool venting; /* (--)
      Internal flag indicating that this vent is actively producing a force.*/
  bool indefinite_duration_warning_sent; /* (--)
      Flag to control the sending of a warning message about opening a vent
      with an unspecified duration.  This warning message is sent only once.
      Default: false. */
  bool allocated_in_set; /* (--)
      Internal flag indicating that this vent was created by the vent-set,
      and needs destroying by the same.  Set only by the VentSet class.
      Default: false*/
  double force_mag;    /* (N)    Magnitude of force from vent */
  double impulse_mag;  /* (N*s)  Magnitude of impulse from vent */
  double duration;     /* (s)    Duration of vent */

  double start_time;   /* (s)   Recorded time when current venting started.*/
  double stop_time;    /* (s)   When to stop venting. */
  double direction[3]; /* (--)
      Force direction unit vector in structural frame of VentSet::dyn_body */
  double force[3];         /* (N)
      Nominal force to be applied, expressed in the structural frame.*/
  double impulse[3];       /* (N*s)
      Nominal impulse expressed in the structural frame.*/
  bool direction_set; /* (--)
      Internal flag indicating that this vent has knowledge of the direction
      in which its force/impulse is applied.*/
  bool force_set; /* (--)
      Internal flag indicating that this vent has knowledge of the force
      to be applied.*/
  bool force_mag_set; /* (--)
      Internal flag indicating that this vent has knowledge of the force
      magnitude to be applied.*/
  bool impulse_set; /* (--)
      Internal flag indicating that this vent has knowledge of the impulse
      to be applied.*/
  bool impulse_mag_set; /* (--)
      Internal flag indicating that this vent has knowledge of the impulse
      magnitude to be applied.*/
  bool duration_set; /* (--)
      Internal flag indicating that this vent has knowledge of the duration
      over which the force will be applied.*/
  double user_set_impulse; /* (N*s)
      User-set impulse magnitude, used for warning when switching to
      impulse mode.i
      Note -- in later edits, values that are automatically generated at or
      before initialization are also assigned to this variable, making it a
      bit of a misnomer; it's purpose remains -- it is an initial
      value used for warning when switching to impulse mode.*/
  double user_set_duration; /* (s)
      User-set duration, used for warning when switching to dynamic mode.
      Note -- in later edits, values that are automatically generated at or
      before initialization are also assigned to this variable, making it a
      bit of a misnomer; it's purpose remains -- it is an initial
      value used for warning when switching to dynamic mode.*/

 public:
  std::string name; /* (--) name of the vent */

  explicit SimpleVent(const double& dyn_time);
  virtual ~SimpleVent() {};

  virtual void use_impulse_mode(bool mode = true);
  void use_dynamic_mode(bool mode = true) { use_impulse_mode(!mode); }

  void set_direction(double direction[3]);
  void set_duration(double duration, bool hold_force = true);
  virtual void set_force_magnitude(double force_, bool hold_impulse = false);
  virtual void set_force_vector(double force_[3], bool hold_impulse = false);
  virtual void set_impulse_magnitude(double impulse, bool hold_force = true);
  virtual void set_impulse_vector( double impulse[3],
                                   bool hold_force_mag = true);

  double get_force_magnitude() { return force_mag; }
  double get_impulse_magnitude() { return impulse_mag; }
  double get_duration() { return duration; }
  double get_start_time() { return start_time; }
  double get_stop_time() { return stop_time; }
  void get_direction(double direction_[3])
          { jeod::Vector3::copy(direction, direction_); }
  void get_force(double force_[3]) { jeod::Vector3::copy(force, force_); }
  void get_impulse(double impulse_[3]) { jeod::Vector3::copy(impulse, impulse_); }
  bool direction_is_set() { return direction_set; }
  bool force_is_set() { return force_set; }
  bool force_magnitude_is_set() { return force_mag_set; }
  bool impulse_is_set() { return impulse_set; }
  bool impulse_magnitude_is_set() { return impulse_mag_set; }
  bool duration_is_set() { return duration_set; }
  bool is_venting() { return venting; }
  bool is_impulse_mode() { return apply_as_impulse; }

  void unset_direction() {direction_set = false;
                          force_set = false;
                          impulse_set = false;}
  void unset_force() {force_mag_set = false;
                      unset_direction();}
  void unset_force_magnitude() {force_mag_set = false;
                                force_set = false;}
  void unset_impulse() {impulse_mag_set = false;
                        unset_direction();}
  void unset_impulse_magnitude() {impulse_mag_set = false;
                                  impulse_set= false;}
  void unset_duration() {duration_set = false;
                         indefinite_duration = true;}

  // Allow access to Vent methods through SimpleVent pointers
  virtual void set_flowrate( double, bool = true);
  virtual void set_exhaust_speed( double, bool = true);
  virtual double get_flowrate();
  virtual double get_exhaust_speed();
  virtual bool flowrate_is_set() { return false; }
  virtual bool exhaust_speed_is_set() { return false; }

 protected:
  // Methods restricted so as to limit access to VentSet only
  void initialize();
  virtual void update();
  virtual bool start_venting();
  virtual void stop_venting();
  virtual void check_status() {}; // No conditional status to check on a simple-
                                  // vent; this method is provided (and
                                  // left empty) for polymorphic extension.
  virtual void update_mass_demand() {};  //  Likewise, this is here (and
                                         // empty) for polymorphic extension

  // Methods restricted because they are intended to be internal methods
  virtual bool check_configuration();
  virtual void update_force() {}; // Force assumed held fixed during venting for
                                  // a simple-vent; this method is provided (and
                                  // left empty) for polymorphic extension.
  void set_impulse_internal();
  void set_duration_internal();
  virtual void set_force_internal();

 private:
  SimpleVent(const SimpleVent&);
  SimpleVent& operator= (const SimpleVent&);
};
#endif