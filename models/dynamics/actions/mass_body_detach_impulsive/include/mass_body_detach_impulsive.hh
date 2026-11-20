/*******************************************************************************
Purpose:
  (Define the class MassBodyDetachImpulsive, the class used for detaching one
   JEOD MassBody object from its parent with a provided impulse.)

Assumptions and Limitations:
  ((Can only be used when the two bodies have been previously attached using
    MassPoint objects)

Library dependencies:
  ((../src/mass_body_detach_impulsive.cc))

Programmers:
  (((Gary Turner) (OSR) (July 2011) (TS21 contribution) (Initial version)))
  (((Jeff Semrau) (Hon) (July 2017) (Updates per code review)))

*******************************************************************************/

#ifndef CML_MASS_BODY_DETACH_IMPULSIVE_HH
#define CML_MASS_BODY_DETACH_IMPULSIVE_HH

#include "jeod/models/dynamics/body_action/include/body_action.hh"

#include <string>

namespace jeod {
class DynBody;
class DynManager;
class MassPoint;
}

/*****************************************************************************
Name: MassBodyDetachImpulsive
Purpose:
  An extension of the JEOD BodyAction concept to detach two bodies and apply
  an impulse to both bodies at their separation.
Notes:
- This is based on the ideas in the JEOD MassBodyDetach BodyAction. However,
  note that mass-bodies have no state so the application of an impulse to a
  MassBody has no effect.
- This was originally written for JEOD 3 in which a JEOD DynBody was an
  extension of a JEOD MassBody. Under this paradigm, detaching 2 JEOD DynBody
  instances is equivalent to detaching 2 JEOD MassBody instances, and the
  application of an impulse to the state of the DynBody instances made sense.
- In JEOD 5, the architecture was changed. A DynBody now HAS a MassBody, and
  detaching 2 DynBody instances requires detaching the MassBody components
  and applying the impulse to only the DynBody. Thus, this class should
  probably be renamed DynBodyDetachIMpulsive, but the name already existed
  and is in general use from JEOD 3 so the name stuck.
*****************************************************************************/
class MassBodyDetachImpulsive : public jeod::BodyAction {
  // Member data
 public:
  double impulse_magnitude{0.0}; /* (N*s)
    The impulse applied during the detach. */

  double impulse_unit_vector[3] {-1.0, 0.0, 0.0}; /* (--)
    The direction of the impulse to be applied to the parent-body.
    This is specified in the reference frame associated with the mass-point
    on the parent body at which the child body is attached.
    If az_rot or el_rot are specified, this vector is computed.
    The impulse applied to the child-body wil be in the opposite direction to
    that applied to the parent body.*/

  double az_rot {0.0};                  /* (rad)
    The azimuth angle defining the direction in which the impulse is to be
    applied to the parent body; the impulse applied to the child body will
    be in the opposite direction.
    This is measured in the reference frame associated with the mass-point
    on the parent body at which the child body is attached. The azimuth angle
    is measured in the y-z plane of said reference frame, from the negative
    y-axis towards the negative-z axis.*/

  double el_rot {0.0};                  /* (rad)
    The elevation angle defining the direction in which the impulse is to be
    applied to the parent body; the impulse applied to the child body will
    be in the opposite direction.
    This is measured in the reference frame associated with the mass-point
    on the parent body at which the child body is attached. The elevation
    angle is measured from projection onto the y-z plane towards the negative
    x-axis*/

  std::string subject_point_name; /* (--)
    The suffix of the name of the subject point. This must match the
    subject_point_name used in the JEOD BodyAttachAligned class. */

  std::string parent_point_name; /* (--)
    The suffix of the name of the point on the parent body.
    This must match the parent_point_name used in the
    JEOD BodyAttachAligned class. */

  double separation_speed{0.0}; /* (m/s)
    The delta-V from the bodies detaching specified by the user instead
    of being calculated in the apply_impulse() method. */

  bool sep_speed_spec_by_user{false}; /* (--)
    Flag used to determine if the delta-V is to be calculated (default), or
    is set by the user and the impulse should be calculated instead. */

 private:
  double linear_impulse_inrtl[3]; /* (N*s)
    The linear impulse vector applied to the parent body, expressed in the
    inertial reference frame.*/

 public:

  MassBodyDetachImpulsive () = default;
  virtual ~MassBodyDetachImpulsive() override = default;
  MassBodyDetachImpulsive( const MassBodyDetachImpulsive& rhs) = delete;
  MassBodyDetachImpulsive & operator = ( const MassBodyDetachImpulsive&) = delete;

  void initialize (jeod::DynManager & dyn_manager) override;
  void apply (jeod::DynManager & dyn_manager) override;

 protected:
  bool compute_impulse(jeod::DynBody &);
  virtual void apply_impulse( jeod::DynBody     & dyn_body,
                              const std::string & mass_point_name);
};

#endif