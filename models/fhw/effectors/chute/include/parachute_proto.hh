/******************************* TRICK HEADER **********************************
PURPOSE:
    (Parachute function prototypes)

REFERENCE:
    (())

ASSUMPTIONS AND LIMITATIONS:
    ((For good or for awesome?))

LIBRARY DEPENDENCY:
     (())
    
PROGRAMMERS:
    (((R.Borland)          (GCS)   (Mar 2015) (We needs this)))               
*******************************************************************************/

#ifndef PARACHUTE_PROTO_HH_
#define PARACHUTE_PROTO_HH_

#include "parachute_exec.hh"

int ap_inertial_states(double veh_cg_sr[3], 
                       double veh_pos[3], 
                       double veh_vel[3], 
                       double veh_omega[3], 
                       double veh_T_body_from_inert[3][3],
                       double veh_T_body_from_sr[3][3], 
                       double ap_sr[3], 
                       double *ap_pos_inert, 
                       double *ap_inert_vel, 
                       double *veh_wxr_vel_inert, 
                       double *ap_rel_pos_sr) ;

int parachute_relstate (PYLD_RELSTATE   *RS_PYLD,
                        PARACHUTE_DATA  *C); 

#endif
