/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute Aerodynamics data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
    (
    ((Jeff Semrau)  (Hon)  (Jan 2011) (Initial Release))
    ((Jeff Semrau)  (Hon)  (10/2012) (lmbp2122) (Implementation of new Line Sail Model))
    ((Jeff Semrau)  (Hon)  (1/2013) (lmbp2290) (Minor update, added ND units for force directions))
    ((Jeff Semrau)  (Hon)  (11/2013)  (lmbp3148) (New variables to improve line sail modeling))
    )
**************************************************************************/

#ifndef PARACHUTE_LINESAIL_H
#define PARACHUTE_LINESAIL_H

typedef struct Line_Sail_Inputs   
{
   double       VairB[3];                         /* (m/s) Payload Velocity Vector */
   double       pyld_T_inertial_to_body[3][3];    /* (--) Payload inertial to body transformation matrix */
   double       chute_T_struct_to_inertial[3][3]; /* (--) Chute structural to inertial transformation matrix */
   double       chute_qbar;                       /* (N/m2) Chute Dynamic Pressure */
   double       chute_inertial_pos[3];            /* (m)  Chute Inertial Position */
   double       pyld_inertial_pos[3];             /* (m)  Payload Inertial Position */

} Line_Sail_Inputs; 

typedef struct Line_Sail_Outputs   
{
   double       drag_SR[3];         /* (N)   Line Sail Drag Vector in chute SR frame   */
} Line_Sail_Outputs;

typedef struct Line_Sail_Params  
{
   double       Cd;                     /* (--)   Line Cd           */
   double       bag_area;               /* (m2)   Bag Area of Chute */
   double       max_bag_area;           /* (m2)   Max Bag Area of Chute */
   double       min_theta;              /* (rad)    Min Theta to have before allowing chute inflation */
   double       delta_S;                /* (m2)   S increment     */

} Line_Sail_Params;

typedef struct Line_Sail_Worl   
{
   double       drag_sf;                           /* (--) Scale Factor on line sail drag */
   double       drag;                              /* (N) Line sail drag magnitude        */
   double       VairB[3];                          /* (m/s) Negative of Payload Velocity Vector */
   double       VairB_mag;                         /* (m/s) Magnitude of Payload Velocity Vector */
   double       chute_rel_Ipos[3];                 /* (m)  Position of chute relative to pyld in inertial frame */
   double       chute_rel_Bpos[3];                 /* (m)  Position of chute relative to pyld in pyld body frame */
   double       chute_rel_Bpos_mag;                /* (m)  Position magnitude of chute relative to pyld */
   double       cp1[3];                            /* (--) Cross Product */
   double       cp1_mag;                           /* (--) Cross Product */
   double       cp2[3];                            /* (--) Cross Product */
   double       force_direction_pbody[3];          /* (--) Force vector in pyld body frame for direction to apply line sail drag */
   double       force_direction_cbody[3];          /* (--) Force vector in chute body frame for direction to apply line sail drag */
   double       force_direction_cbody_mag;         /* (--) Force vector magnitude */
   double       force_direction_cbody_uvec[3];     /* (--) Force unit vector in chute body frame for direction to apply line sail drag */
   double       force_direction_inertial[3];       /* (--) Force vector in inertial frame for direction to apply line sail drag */
   double       arg1;                              /* (--) */
   double       theta;                             /* (rad)  angle between chute position and pyld velocity vectors */

} Line_Sail_Work;
   

typedef struct LINE_SAIL_DATA {
   Line_Sail_Inputs     input;
   Line_Sail_Outputs    output;
   Line_Sail_Params     param;
   Line_Sail_Work       work;
  
} LINE_SAIL_DATA; 

#ifdef __cplusplus
extern "C" {
#endif

void parachute_line_sail (LINE_SAIL_DATA   *line_sail);

#ifdef __cplusplus
}
#endif

#endif
