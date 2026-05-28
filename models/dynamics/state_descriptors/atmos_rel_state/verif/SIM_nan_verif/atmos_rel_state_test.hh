/********************************* TRICK HEADER *******************************
PURPOSE:
   (Subclass of AtmosRelativeState for test specific "corner areas" of model.)

PROGRAMMERS:
     (((Robert Phillips) (OSR) (Feb 19) (ANTARES) (created initial version)))
****************************************************************************** */

#ifndef ANTARES_ATMOS_REL_STATE_TEST_HH
#define ANTARES_ATMOS_REL_STATE_TEST_HH

#include "../../include/atmos_relative_state.hh"


class AtmosRelStateTest : public AtmosRelativeState
{
public:

   AtmosRelStateTest(
     const jeod::DynBody       & body_ref,
     ExtendedPlanetaryDerivedState & pds_ref,
     AtmosphereExecInterface       & atmos_exec_ref);
   virtual ~AtmosRelStateTest(){};

   void compute_euler_angles();

   double ryp_vector[3];

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies
  AtmosRelStateTest (const AtmosRelStateTest &);
  AtmosRelStateTest & operator= (const AtmosRelStateTest &);
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
