/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute attach point damping data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
     (((Jeff Semrau) (Honeywell) (Aug 2011) (Initial Version)))
*************************************************************************/

#ifndef PARACHUTE_ATT_PNT_H_
#define PARACHUTE_ATT_PNT_H_

struct Att_Pnt_Inputs { 
   double     pyld_att_pnt_moment_arm[3];     /* (m)   Moment arm between payload CG and attach point */
   double     pyld_body_rates[3];             /* (rad/s) Payload body rates                             */
   double     VairB[3];                       /* (m/s) Payload Velocity Vector                        */
   double     load_mag;                       /* (N)   Riser load magnitude                           */
   double     pyld_T_body_from_SR[3][3];      /* (--)  Payload SR to Body Transformation Matrix       */
   double     num_chutes;                     /* (--)  Number of chutes in cluster                    */
};

struct Att_Pnt_Outputs {
   double     att_pnt_force_SR[3];            /* (N)   Riser force vector                             */
   double     att_pnt_moment_SR[3];           /* (N*m) Riser moment vector                            */
}; 

struct Att_Pnt_Work {
   double      att_pnt_moment_arm_body[3];    /* (m)   Moment arm in body reference frame                */
   double      att_pnt_vel_rel[3];            /* (m/s) Attach Point velocity vector due to payload rate  */
   double      att_pnt_vel_body[3];           /* (m/s) Attach point velocity in body frame               */
   double      att_pnt_vel_mag;               /* (m/s) Attach point velocity magnitude                   */
   double      att_pnt_force_body[3];         /* (N)   Riser force vector in payload body frame          */
};

struct ATT_PNT_DATA {
   Att_Pnt_Inputs   input;                    /* (--)  */
   Att_Pnt_Outputs  output;                   /* (--)  */
   Att_Pnt_Work     work;                     /* (--)  */
};

void parachute_att_pnt_damping (ATT_PNT_DATA&   apd);

#endif
