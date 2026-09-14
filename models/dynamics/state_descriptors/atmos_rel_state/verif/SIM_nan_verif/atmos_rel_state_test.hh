/********************************* TRICK HEADER *******************************
PURPOSE:
   (Subclass of AtmosRelativeState for test specific "corner areas" of model.)

PROGRAMMERS:
     (((Robert Phillips) (OSR) (Feb 19) (ANTARES) (created initial version)))
****************************************************************************** */

#ifndef CML_ATMOS_REL_STATE_TEST_HH
#define CML_ATMOS_REL_STATE_TEST_HH

#include "../../include/atmos_relative_state.hh"
#include "jeod/models/utils/orientation/include/orientation.hh"
#include "jeod/models/utils/math/include/vector3.hh"


class AtmosRelStateTest : public AtmosRelativeState
{
public:

   AtmosRelStateTest(
     const jeod::DynBody       & body_ref,
     ExtendedPlanetaryDerivedState & pds_ref,
     AtmosphereExecInterface       & atmos_exec_ref);
   ~AtmosRelStateTest() override = default;
   AtmosRelStateTest (const AtmosRelStateTest &) = delete;
   AtmosRelStateTest & operator= (const AtmosRelStateTest &) = delete;

   void compute_euler_angles();

   double ryp_vector[3];
};


inline AtmosRelStateTest::AtmosRelStateTest( 
  const jeod::DynBody       & body_ref,
  ExtendedPlanetaryDerivedState & pds_ref,
  AtmosphereExecInterface       & atmos_exec_ref):
  AtmosRelativeState( body_ref, pds_ref, atmos_exec_ref ),
  ryp_vector() 
  {
  }


inline void AtmosRelStateTest::compute_euler_angles()
{
  jeod::Orientation::compute_euler_angles_from_matrix( 
    T_traj_body,
    jeod::Orientation::Roll_Yaw_Pitch,
    traj_to_body_RYP);
  check_euler_for_nan();
  jeod::Vector3::copy( traj_to_body_RYP, ryp_vector );
}



#endif