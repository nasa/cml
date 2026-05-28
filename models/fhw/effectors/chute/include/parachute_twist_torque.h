/********************************** TRICK HEADER **************************
PURPOSE:
    (Parachute Twist Model data structure)

REFERENCE:
    (( -- ))

ASSUMPTIONS AND LIMITATIONS:
    (( -- ))

PROGRAMMERS:
     (
     ((Jeff Semrau)  (HW)   (Feb 2010)    (Initial Version))
     ((Jeff Semrau)  (HW)   (May 2011)    (LMBP 683))
     ((Brian Bihari) (ESCG) (26-Apr-2012) (Main Flyout Model updates per LRS Model Memo 9    ))
     ((Jeff Semrau)  (HW)   (04/2015)    (CR2673) (Updated torque model for textile risers))
     )
**************************************************************************/

#ifndef PARACHUTE_TWIST_TORQUE_H
#define PARACHUTE_TWIST_TORQUE_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Twist_Inputs
{
    double roll_rate;                        /* (rad/s)  CM Roll Rate                                         */
    double dt;                               /* (s)      Time Step for local simple Euler Integrator          */
    double inertial_vector[3];               /* (--)     Inertial Unit Vector of Riser Line Force             */
    double T_ECI_to_Body[3][3];              /* (--)     Transformation Matrix from Inertial to CM Body Frame */
    double T_Body_to_SR[3][3];               /* (--)     Transformation Matrix from CM Body to CM SR Frame    */
    int    n;                                /* (--)     Total number of sine waves                           */
    double cev_weight;                       /* (N)      Total system weight                                  */
    double main_diameter;                    /* (m)      Parachute Diameter                                   */
    bool   chutes_inflated;                  /* (--)     Flag to signal that chutes are inflated              */
    double riser_load;                       /* (N)      Total Riser Load */
} Twist_Inputs;


typedef struct Twist_Outputs 
{
    double twist_torque_SR[3];               /* (N*m)    Parachute Line Twist Torque in CM SR frame           */
    double CdS;                              /* (m2)     Parachute Projected Area from TCD code               */
} Twist_Outputs;


typedef struct Twist_Params 
{
    int    num_chutes;                       /* (--)      Total number of parachutes                           */
    double Rf;                               /* (m)       Fairlead characteristic radius                       */
    double Rw;                               /* (m)       Riser radius                                         */
    double k;                                /* (--)      Riser realignment factor                             */
    double t_trans;                          /* (s)       Transient to Damped Epoch TCD start time             */
    double delta_t_trans;                    /* (s)       Time period to transition from Transient to Damped TCD Epochs */
    double S_transition_time;                /* (s)       Time period to transition from current to TCD chute area      */
    double initial_twist_angle;              /* (rad)     Initial twist angle                                  */

    // Rf declarations
    double flyout_angle_3main_table[10];    /* (rad)      Flyout angle table for 3 mains                       */
    double flyout_angle_2main_table[10];    /* (rad)      Flyout angle table for 2 mains                       */
    double Rf_3main_table[10];              /* (m)        Fairlead radius table for 3 mains                    */
    double Rf_2main_table[10];              /* (m)        Fairlead radius table for 2 mains                    */
    int    index_Rf;                        /* (--)       Index for fairlead radius table                      */
    
    //Rw Declarations
    double Rw_3main_table[10];              /* (m)        Winding radius table for 3 mains                     */
    double Rw_2main_table[10];              /* (m)        Winding radius table for 2 mains                     */
    int    index_Rw;                        /* (--)       Index for winding radius table                       */

    //Kp declarations
    double Kp_3main_table[10];              /* (--)       Phi coefficient table for 3 mains                    */
    double Kp_2main_table[10];              /* (--)       Phi coefficient table for 2 mains                    */
    int    index_Kp;                        /* (--)       Index for phi coefficient table                      */
    
    //phi0 declarations
    double phi0_3main_table[10];            /* (rad)      Phi offset table for 3 mains                         */
    double phi0_2main_table[10];            /* (rad)      Phi offset table for 2 mains                         */
    int    index_phi0;                      /* (--)       Index for phi offset table                           */

} Twist_Params;


typedef struct Twist_Work
{
    bool    twist_torque_init;               /* (--)      Twist Torque Initialization Flag                     */
    double  t;                               /* (s)       Time elapsed since model initiation                  */
    double  twist_angle;                     /* (rad)     Twist Angle                                          */
    double  prev_twist_angle;                /* (rad)     Twist Angle Previous Cycle                           */
    double  attach_radius;                   /* (m)                                                            */
    double  twist_torque_mag;                /* (N*m)     Twist Torque Magnitude                               */
    double  twist_torque_inertial[3];        /* (N*m)     Twist Torque in Inertial Frame                       */
    double  twist_torque_body[3];            /* (N*m)     Twist Torque in CM Body Frame                        */
    double  flyout_angle;                    /* (rad)     Flyout angle                                         */
    double  phi_star;                        /* (rad)     Twist Transition Angle                               */
    double  WT;                              /* (N)       Total system weight                                  */
    double  direction;                       /* (--)      Direction                                            */
    double  S;                               /* (m2)      Parachute Reference Surface Area                     */

    double fblend;                           /* (--) */
    double Sp;                               /* (m2)      Computed Projected Chute Drag Area                   */
    double CdSF;                             /* (1/m2)    Chute Drag Area scale factor                         */
    double Chute_Area;                       /* (m2)      Computed projected chute area                        */
    double start_time;                       /* (s)       Start time when transition to TCD Area begins        */
    double elasped_time;                     /* (s)       Elasped time of TCD transition                       */
    double delta_S[3];                       /* (m2)      Delta in Steady State Area to TCD Area               */
    double cycle_count;                      /* (--)      Number of cycles to do area transition               */
    double S_step[3];                        /* (m2)      Step change in chute area                            */

    double Rf;                               /* (m)       Fairlead Radius                                      */
    double Rw;                               /* (m)       Winding Radius                                       */
    double Kp;                               /* (--)      Formation Frequency                                  */
    double phi0;                             /* (rad)     Phase offset                                         */
    double phi_star_fwd;                     /* (rad)     Forward Twist Formation Angle                        */
    double phi_star_rev;                     /* (rad)     Reverse Twist Formation Angle                        */
    double phi_check;                        /* (--)      */
    int    rate_sign;                        /* (--)      Sign of twist rate                                   */
    int    twist_sign;                       /* (--)      Sign of twist angle                                  */
    int    phi_phi0_sign;                    /* (--)      Sign to indicate if phi/phi0 are same sign           */

    bool fwd_twist_transition;               /* (--)      Flag to indicate twist formation has occurred        */
    bool fwd_twist;                          /* (--)      Fwd twist or reverse twist                           */
    bool hysteresis_flag;                    /* (--)      Twist Hysteresis on or off flag                      */
    double twist_check;                      /* (--)      Check of twist angle for hysteresis                  */
    double riser_uvec[3];                    /* (--)      Unit Vector of riser                                 */

} Twist_Work;


typedef struct Waveform_Work 
{
    double flyout_angle;                    /* (rad)     Flyout angle                            */
    double t;                               /* (s)       Time elapsed since model initiation     */
    double Sp;                              /* (m2)      Projected Chute Area                    */
    double Sp_delta;                        /* (m2)      Change in projected chute area          */
    double mod_check;                       /* (--)      Intermediate var to hold modulus check of time */
} Waveform_Work;

typedef struct Waveform_Params
{
    int    flyout_range;          /* (--)    Number of indices for flyout computations */
    int    Sp_range;              /* (--)    Number of indices for Sp computations     */
    double flyout_bias;           /* (rad)   Initial value of flyout angle             */
    double flyout_A[3];           /* (rad)   Flyout Angle sine wave peak amplitude     */
    double flyout_delta_t[3];     /* (s)     Flyout Angle sine wave time shift         */
    double flyout_T[3];           /* (s)     Flyout Angle sine wave period             */
    double flyout_Anought[3];     /* (rad)   Flyout Angle sine wave amplitude shift    */
    double Sp_bias;               /* (m2)    Projected Chute Area initial value        */
    double Sp_A[2];               /* (m2)    Projected Chute Area sine wave peak amplitude  */
    double Sp_delta_t[2];         /* (s)     Projected Chute Area sine wave time shift      */
    double Sp_T[2];               /* (s)     Projected Chute Area sine wave time period     */
    double Sp_Anought[2];         /* (m2)    Projected Chute Area sine wave amplitude shift */
    double Sp_K[2];               /* (--)    Projected Chute Area gain                      */
    double Sp_CdSF;               /* (1/m2)  Projected Chute Area scale factor              */
} Waveform_Params;

typedef struct WAVEFORM_DATA {
    Waveform_Params     param;
    Waveform_Work       work;
} WAVEFORM_DATA;


typedef struct TWIST_DATA {
    Twist_Inputs     input;
    Twist_Outputs    output;
    Twist_Params     param;
    Twist_Work       work;
    WAVEFORM_DATA    transient;
    WAVEFORM_DATA    damped;
} TWIST_DATA;

void parachute_twist_torque (TWIST_DATA   *T);

void parachute_waveform (WAVEFORM_DATA   *T);

#ifdef __cplusplus
}
#endif

#endif
