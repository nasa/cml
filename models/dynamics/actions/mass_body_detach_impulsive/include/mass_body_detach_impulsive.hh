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

#ifndef JEOD_MASS_BODY_DETACH_IMPULSIVE_HH
#define JEOD_MASS_BODY_DETACH_IMPULSIVE_HH

#include "jeod/models/dynamics/mass/include/class_declarations.hh"

// Model includes
#include "jeod/models/dynamics/body_action/include/class_declarations.hh"
#include "jeod/models/dynamics/body_action/include/body_action.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

namespace jeod {
class DynBody;
class MassPoint;
}

// Causes the subject body to detach from its parent with a specified impulse.
class MassBodyDetachImpulsive : public jeod::BodyAction {
  // Member data
  public:
    double impulse_magnitude;       /* (N*s)
      The impulse applied during the detach. */

    std::string subject_point_name; /* (--)
      The suffix of the name of the subject point. This must match the
      subject_point_name used in the JEOD BodyAttachAligned class. */

    std::string parent_point_name;  /* (--)
      The suffix of the name of the point on the parent body.
      This must match the parent_point_name used in the
      JEOD BodyAttachAligned class. */

    double separation_speed;        /* (m/s)
      The delta-V from the bodies detaching specified by the user instead
      of being calculated in the apply_impulse() method. */

    bool sep_speed_spec_by_user;    /* (--)
      Flag used to determine if the delta-V is to be calculated (default), or
      is set by the user and the impulse should be calculated instead. */

  // Member functions
    // Default constructor.
    MassBodyDetachImpulsive ();

    // Destructor.
    virtual ~MassBodyDetachImpulsive() {};

    // initialize: Initialize the initializer.
    virtual void initialize (jeod::DynManager & dyn_manager);

    // apply: Detach the body from its parent body.
    virtual void apply (jeod::DynManager & dyn_manager);

  protected:
    virtual void apply_impulse( jeod::DynBody          & dyn_body,
                                std::string  mass_point_name);

  private:
    // not implemented
    MassBodyDetachImpulsive( const MassBodyDetachImpulsive& rhs);
    MassBodyDetachImpulsive & operator = ( const MassBodyDetachImpulsive&);
};

#endif
