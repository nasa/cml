/*******************************TRICK HEADER******************************
PURPOSE: (Definition of the bodies in the wake profile.)

LIBRARY DEPENDENCY:
   ((../src/wake_bodies.cc))

PROGRAMMERS:
  ( ((Gary Turner) (OSR) (Mar 2016) (Antares)
                (New adaptation to meet Antares coding standards))
    ((Bingquan Wang) (OSR) (Oct 2016) (Antares)
                (Library dependency warning fixing))
    ((G Turner) (OSR) (June 2019) (Added "no units" specification for Trick17)))
**********************************************************************/
#ifndef WAKE_BODIES_HH
#define WAKE_BODIES_HH

#include "wake_params.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

/*****************************************************************************
WakeGeneratingBody
Purpose:(The body that is generating the wake)
*****************************************************************************/
class WakeGeneratingBody
{
 friend class SubsonicWake;
 friend class WakeFollowingBody;
 friend class WakeEffectsBase;
 friend class WakePrfModel;
 friend class WakeReverseFlow;

 protected: // External references
  const double * inertial_pos; /* (m) inertial position. 3-vector */
  const double * inertial_vel; /* (m/s) inertial velocity. 3-vector */
  const double * CoM_wrt_struc;/* (m) posn of CoM in struc frame. 3-vec */
  const double (& T_inrtl_to_body)[3][3];/* (--) inrtl->body transformation mx*/
  const double (& T_struc_to_body)[3][3];/* (--) struc->body transformation mx*/
  const double * freestream_vel; /* (m/s)
         freestream velocity in body-A frame. Clarification -- this is the
         velocity of the body with respect to the freestream. 3-vector. */
  const double & freestream_mach;/* (--)
         mach number associated with freestream velocity. */
  const double & freestream_density; /* (kg/m3) density of freestream flow. */
  const double & total_alpha; /* (rad) total angle of attack. */

 public: // For use by effect models
  double freestream_vel_mag; /* (m/s) magnitude of freestream velocity. */
  double unitvec_freestream[3]; /* (--) unit vector aligned with freestream */

  WakeGeneratingBody(
      const double * inertial_pos,
      const double * inertial_vel,
      const double * CoM_wrt_struc,
      const double (& T_inrtl_to_body)[3][3],
      const double (& T_struc_to_body)[3][3],
      const double * freestream_vel,
      const double & freestream_mach,
      const double & freestream_density,
      const double & total_alpha);
  virtual ~WakeGeneratingBody(){};

  void compute_freestream_vel_mag();
  double get_total_alpha(){return total_alpha;};

 private: // and undefined:
  WakeGeneratingBody (const WakeGeneratingBody& rhs);
  void operator = (const WakeGeneratingBody& rhs);
};

/*****************************************************************************
WakeFollowingBody
Purpose:(Replaces the old OBJECT struct.  Defines the characteristics of the
         vehicle-like object.)
*****************************************************************************/
class WakeFollowingBody
{
 protected: // External references
  const double * grid_origin; /* (m) copied from parameters at initialization.*/
  bool generate_force; /* (--) whether to calculate the wake force on body.*/
  const double & area; /* (m2)
       Area of the object in the wake, perpendicular to the flow direction*/
  const double * inertial_pos;  /* (m) inertial position. 3-vector */
  const double * inertial_vel;  /* (m/s) inertial velocity. 3-vector */
  WakeGeneratingBody & objectA; /* (--) the wake-generating body*/

 public: // For WakeEffectsBase to access
  double  separation_distance; /* (m)
       Absolute separation distance of the object from the wake-producing body*/
  double  trailing_distance;   /* (m)
       Trailing distance of the object along the wake axis of symmetry */
  double  radial_distance;     /* (m)
       Radial distance of the object from the wake axis of symmetry */
  double  axial_sep_vel;       /* (m/s)
       Axial separation velocity of the object in the wake */

  double pos_wrt_grid_in_bodyA[3]; /* (m)
       Position of body wrt objectA's grid-point.  Left public so this can be
       accessed by the wake-effect when recomputing the radial distance
       following a grid-offset.*/

 protected: // Interim internal use only
  double pos_wrt_objectA_bodyA[3];  /* (m)
       Relative position of the object in the bodyA reference frame*/
  double pos_wrt_objectA_inrtl[3];  /* (m)
       Relative position of the object in the inertial reference frame*/

 public:
  WakeFollowingBody(const double & area_in,
                    const double * inertial_pos,
                    const double * inertial_vel,
                    WakeGeneratingBody & objectA_in,
                    const double * grid_origin_in);
  virtual ~WakeFollowingBody(){};

  double get_area() const {return area;};
  bool get_generate_force() const { return generate_force;};
  virtual double get_drag_area() const {return 0.0;};

  virtual void compute_relative_state();

  virtual void compute_force(double force_mag){(void)force_mag;};
  virtual void set_generate_force(bool target) = 0;

 private: // and undefined:
  WakeFollowingBody (const WakeFollowingBody& rhs);
  void operator = (const WakeFollowingBody& rhs);
};

/*****************************************************************************
WakeFollowingBodyNoForce
Purpose:(Instantiable class for a following body that has no requirement for
         computing the wake-force)
*****************************************************************************/
class WakeFollowingBodyNoForce : public WakeFollowingBody
{
 public:
  WakeFollowingBodyNoForce( const double & area_in,
                            const double * inertial_pos,
                            const double * inertial_vel,
                            WakeGeneratingBody & objectA_in,
                            const double * grid_origin_in);
  virtual ~WakeFollowingBodyNoForce(){};
  virtual void set_generate_force(bool target) override;

 private:
  WakeFollowingBodyNoForce (const WakeFollowingBodyNoForce& rhs);
  void operator = (const WakeFollowingBodyNoForce& rhs);
};
/*****************************************************************************
WakeFollowingBodyWithForce
Purpose:(Instantiable class for a following body that has potential
         requirement for computing the wake-force)
*****************************************************************************/
class WakeFollowingBodyWithForce : public WakeFollowingBody
{
 protected: // External references
  const double (& T_inrtl_to_body)[3][3]; /* (--) inrtl->body transformation mx */
  const double (& T_struc_to_body)[3][3]; /* (--) struc->body transformation mx */

 public:
  // Model input
  double Cd;        /* (--)
       Coefficient of Drag for the object in the wake of the main body  */

  // Model output
  double force[3];  /* (N) output force in struc frame.*/

 public:
  WakeFollowingBodyWithForce(
               const double & area_in,
               const double * inertial_pos,
               const double * inertial_vel,
               const double (& T_inrtl_to_body_in)[3][3],
               const double (& T_struc_to_body_in)[3][3],
               WakeGeneratingBody & objectA_in,
               const double * grid_origin_in);

  virtual void set_generate_force(bool target) override;
  virtual void compute_relative_state() override;
  virtual void compute_force(double force_mag) override;
  virtual double get_drag_area() const override {return Cd * area;};

 private:
  WakeFollowingBodyWithForce (const WakeFollowingBodyWithForce& rhs);
  void operator = (const WakeFollowingBodyWithForce& rhs);
};
#endif