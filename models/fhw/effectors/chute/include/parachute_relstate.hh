/******************************* TRICK HEADER **********************************
PURPOSE:
    (Definition of relative state related data for Orion parachute model)

REFERENCES:
    ((none))

ASSUMPTIONS AND LIMITATIONS:
    ((none))

PROGRAMMERS:
    (((C. Holtz, A. Barth)   (LMSSS)   (04-01-09)   (Initial Version for hi-fi chute model))
    (((J. Semrau)            (HON)     (May 2010)   (Updates for CR 763)))                    
    (((J. Semrau)            (HON)     (Aug 2010)   (Updates for DR 824)))                    
    (((J. Semrau)            (HON)     (Jan 2011)   (Updates for CR 859)))                    
    (((J. Semrau)            (HON)     (Nov 2013)   (Updates for LMBP #3148, new code to improve confluence fitting modeling)))  
    (((J. Semrau)            (HON)     (Nov 2015)   (CR4609, Cleaned up define statements, replaced with static const)))  
    )
*******************************************************************************/

#ifndef __RELSTATE_DATA__
#define __RELSTATE_DATA__

#include "parachute_max_items.hh"

/************************************************************/
/**************** Chute Data Structure  *********************/
/************************************************************/

struct CHUTE_RELSTATE_IN       
{
   /* Relstate Inputs */
   double chute_pos_inert[3] ;             /* (m)     Chute i inertial position */
   double chute_vel_inert[3] ;             /* (m/s)   Chute i inertial velocity */
   double chute_inert_rate[3] ;            /* (rad/s) Chute i body rates wrt to inertial expressed in inertial frame */
   double chute_T_body_from_inert[3][3] ;  /* (--)    Transform from inertial to chute i body frame */
   double chute_wind_vel_inert[3] ;        /* (m/s)   Inertial wind velocity for chute i */
   double chute_atm_density ;              /* (kg/m3) Atmospheric density for chute i */
   double chute_speed_of_sound ;           /* (m/s)   Speed of sound for chute i */
   double chute_radius;                    /* (m)     Current radius of chute */

};

struct CHUTE_RELSTATE_PARAM             
{
   /* Relstate Parameters */
   double chute_T_body_from_sr[3][3] ;           /* (--)   Transform from chute SR frame to chute body frame */
   double chute_T_aero_from_sr[3][3] ;           /* (--)   Transform from chute SR frame to chute aero database frame */

};

struct CHUTE_RELSTATE_WORK       
{
   /* Relstate work variables */
   double riser_line_inertial_dv[3] ;   /* (m/s)    Relative inertial velocity between the ends of riser line i */ 

   double chute_sep_distance[3] ;       /* (m)      Separation distance between payload and chute */

   double chute_att_pnt_clearance[3];   /* (m)      Position vector from confluence point to chute */
   double chute_att_pnt_clearance_mag;  /* (m)      Magnitude of distance from confluence point to chute */

   int    init_flag;                    /* (--)     Initialization flag */

   /* Param to compute line stretch from CF to Chute CG */
   double riser_suspension_inertial_dv[3]; /* (m/s) Relative inertial velocity between Chute CG and attach point */

};

struct CHUTE_RELSTATE_OUT           
{
   /* Relstate outputs */
   double chute_cg_sr[3] ;              /* (m)      Chute i CG location in chute SR frame */
   double chute_moment_arm[3];          /* (m)      Moment arm from suspension line confluence to chute CG */
   double chute_att_pnt_pos_inert[3] ;  /* (m)      Inertial position of payload attach point i */
   double chute_att_pnt_vel_inert[3] ;  /* (m/s)    Inertial velocity of payload attach point i */
   double chute_alpha_total ;           /* (rad)    Total angle of attack for chute i */
   double chute_dynamic_pressure ;      /* (N/m2)   Dynamic pressure for chute i */
   double chute_mach ;                  /* (--)     Mach number for chute i */
   double chute_wrel_vel_inert[3] ;     /* (m/s)    Wind relative velocity of chute i in the inertial frame */
   double chute_wrel_vel_body[3] ;      /* (m/s)    Wind relative velocity of chute i in the body frame */
   double chute_wrel_vel_sr[3] ;        /* (m/s)    Wind relative velocity of chute i in the SR frame */
   double chute_wrel_vel_aero[3] ;      /* (m/s)    Wind relative velocity of chute i in the Aero frame */
   double chute_wrel_vel_mag ;          /* (m/s)    Magnitude of wind relative velocity of chute i */
   double chute_wrel_vel_uvec_sr[3] ;   /* (m/s)    Wind relative velocity unit vector of chute i in the SR frame */
   double chute_ap_force_vec_inert[3] ; /* (--)     Inertial direction vector of force applied to chute i att point */
   double chute_wxr_vel_inert[3] ;      /* (m/s)    Rotational velocity component (w X r) for chute i in inertial frame */
   double riser_line_distance ;         /* (m)      Distance between the ends of riser line i */
   double riser_line_stretch_rate ;     /* (m/s)    Relative speed between the ends of riser line i along the length vector */ 
   
   double chute_sep_distance_mag ;      /* (m)      Separation distance between payload and chute */
   bool   chute_line_stretch ;          /* (--)     Indicator that the chute has reached nominal line stretch */       
   bool   canopy_extracted;             /* (--)     Flag that indicates that the canopy has been fully extracted from the bag */

   /* New Vars to compute line stretch from CF to Chute CG */
   double chute_cg_force_vec_inert[3];     /* (--)  Inertial Direction vector from Chute CG to Attach Point */
   double riser_suspension_distance;       /* (m)   Distance from Chute CG to Attach Point                  */
   double riser_suspension_stretch_rate;   /* (m/s) Relative speed between Chute CG and attach point        */
   bool   riser_susp_line_stretch;         /* (--)  Flag that indicates chute riser-suspension lines have reached line stretch */

};

struct CHUTE_RELSTATE                     
{
   CHUTE_RELSTATE_IN     input ;
   CHUTE_RELSTATE_PARAM  param ;
   CHUTE_RELSTATE_WORK   work ;
   CHUTE_RELSTATE_OUT    output ;

};

/************************************************************/
/************** Confluence Data Structure  ******************/
/************************************************************/
struct CONFL_RELSTATE_IN      
{
   /* Relstate Inputs */
   double confl_pnt_pos_inert[3] ;                          /* (m)     Confluence point CG inertial postion vector */
   double confl_pnt_vel_inert[3] ;                          /* (m/s)   Confluence point CG inertial velocity vector */

};

struct CONFL_RELSTATE_WORK            
{
   double confl_pnt_pos_inert_cg_rel[3]; /* (m) Confluence point CG position relative to the payload CG in the inertial frame */
   double confl_pnt_pos_body_cg_rel[3]; /* (m) Confluence point CG position relative to the payload CG in the payload body frame */
   double confl_pnt_pos_sr_cg_rel[3]; /* (m) Confluence point CG position relative to the payload CG in the payload SR frame */

};

struct CONFL_RELSTATE_OUT      
{
   double confl_pnt_pos_sr[3]; /* (m) Confluence point CG position vector in the payload SR frame */
   double confl_pnt_vel_inert_cg_rel[3]; /* (m/s) Confluence point CG velocity with respect to the payload CG in the inertial frame */

};

struct CONFL_RELSTATE         
{
   CONFL_RELSTATE_IN     input ;
   CONFL_RELSTATE_WORK work;
   CONFL_RELSTATE_OUT  output;

};

/************************************************************/
/*************** Payload Data Structure  ********************/
/************************************************************/
struct PYLD_RELSTATE_IN        
{
   /* Relstate Inputs */
   double pyld_pos_inert[3] ;                          /* (m)     Payload CG inertial postion vector */
   double pyld_vel_inert[3] ;                          /* (m/s)   Payload CG inertial velocity vector */
   double pyld_inert_rate[3] ;                         /* (rad/s) Payload body rates wrt to inertial expressed in inertial frame */
   double pyld_T_body_from_inert[3][3] ;               /* (--)    Transform from inertial to payload body frame */
   double pyld_cg_sr[3] ;                              /* (m)     Payload CG loc in payload SR frame */

   //Inputs for Pilot Payload States
   double pilot_pyld_pos_inert[3][3];                  /* (m)     Pilot Payload CG inertial position vector */
   double pilot_pyld_vel_inert[3][3];                  /* (m/s)   Pilot Payload CG inertial velocity vector */
   double pilot_pyld_inert_rate[3][3];                 /* (rad/s) Pilot Payload body rates wrt to inertial expressed in inertial frame */

};

struct PYLD_RELSTATE_PARAM             
{
   /* Relstate Parameters */
   double pyld_att_pnt_sr[MAX_ITEMS::MAX_ATTACH_PTS][3] ;           /* (m)    Payload attach point location in payload SR frame */
   double pyld_T_body_from_sr[3][3] ;            /* (--)   Transform from payload SR to payload body frame */

};

struct PYLD_RELSTATE_WORK         
{
   /* Relstate work variables */
   double pyld_att_pnt_wxr_vel_inert[MAX_ITEMS::MAX_ATTACH_PTS][3] ;   /* (m/s)    Rotational velocity component (w X r) for attach point i in inertial frame */

   double harness_line_inertial_dv[MAX_ITEMS::MAX_HLINES][3] ; /* (m/s)    Relative inertial velocity between the ends of harness line i */ 

};

struct PYLD_RELSTATE_OUT        
{
   /* Relstate outputs */
   double pyld_att_pnt_pos_inert[MAX_ITEMS::MAX_ATTACH_PTS][3] ;       /* (m)      Inertial position of payload attach point i */
   double pyld_att_pnt_vel_inert[MAX_ITEMS::MAX_ATTACH_PTS][3] ;       /* (m/s)    Inertial velocity of payload attach point i */
   double pyld_ap_force_vec_inert[MAX_ITEMS::MAX_ATTACH_PTS][3] ;      /* (--)     Inertial direction vector of force applied to payload att point i */
   double pyld_att_pnt_moment_arm[MAX_ITEMS::MAX_ATTACH_PTS][3] ;      /* (m)      Moment arm of payload attach point to payload CG */
   double harness_line_distance[MAX_ITEMS::MAX_HLINES] ;       /* (m)      Distance between the ends of harness line i */
   double harness_line_stretch_rate[MAX_ITEMS::MAX_HLINES] ;   /* (m/s)    Relative speed between the ends of harness line i along the length vector */ 
   bool   harness_line_stretch[MAX_ITEMS::MAX_HLINES] ;        /* (--)     Indicator that the harness line has reached nominal line stretch */       

};

struct PYLD_RELSTATE               
{
   PYLD_RELSTATE_IN     input ;
   PYLD_RELSTATE_PARAM  param ;
   PYLD_RELSTATE_WORK   work ;
   PYLD_RELSTATE_OUT    output ;

};
   
/************************************************************/
/*********** Combined RelState Data Structure  **************/
/************************************************************/
struct RELSTATE_DATA             
{
  CHUTE_RELSTATE     chute[MAX_ITEMS::MAX_CHUTES] ;
  CONFL_RELSTATE     confl ;
  PYLD_RELSTATE      pyld ;
   
};

#endif
