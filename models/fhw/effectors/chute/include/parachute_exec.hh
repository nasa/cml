/* Trick Header

PURPOSE:

     (This header file contains all include files, definitions
     and structure definitions needed internally only by the
     parachute model {without dispersions}.)                        

PROGRAMMERS:
    (
    ((Jeff Semrau) (HW) (26-Mar-09) (Initial Release))
    ((Jeff Semrau) (HW) (May 2010)  (Updated for CR 763))
    ((Gesting) (OSR) (05/2010) (791) (Update OIMU to Honeywell updates))
    ((Jeff Semrau) (HW) (06/2010) (806) (Add flag to turn off computation of debug data))
    ((Jeff Semrau) (HW) (08/2010) (824) (New declarations for analysis vars))
    ((Jeff Semrau) (HW) (01/2011) (859) (New declarations for line sail, monitoring code))
    ((Jeff Semrau) (HW) (05/2011) (LMBP 683) (New twist torque model))
    ((Jeff Semrau) (HW) (07/2011) (LMBP 934) (New declarations for skipped 2nd stage and instantaneous CD))
    ((Jeff Semrau) (HON) (02/2012) (lmbp1395) (Chute Instantaneous Cd (ICD)))
    ((Jeff Semrau) (HON) (05/2012) (lmbp1650) (Implementing LRS MMv9 updates))
    ((Jeff Semrau) (HON) (10/2012) (lmbp2122) (New Skip Stage vars, added PEC to cut chutes in I/F))
    ((Jeff Semrau) (HON) (08/2013) (lmbp2720) (Added new vars to track qbar at peak loads for LRS))
    ((Jeff Semrau) (HON) (11/2013) (lmbp3148) (Added new vars to randomly fail a chute and improve confluence fitting modeling))
    ((Jeff Semrau) (HON) (10/2015) (CR4460) (Updates for new monitoring for LRS))
    ((Jeff Semrau) (HON) (11/2015) (CR4R609 (Updates for new integration method))
    ((Gary Turner) (OSR) (06/2019) (Antares)
                         (Scoped the nested enum structs to give visibility
                          through SWIG for logging in Trick17))
    )
LIBRARY DEPENDENCY:
   ((parachute_exec_init.o)
    (parachute_mass_init.o)
    (parachute_exec.o)
    (parachute_recontact_monitor.o))

*/

#include "mortar.h"
#include "parachute_max_items.hh"
#include "parachute_inflation.h"
#include "parachute_coupling.h"
#include "parachute_aero.h"
#include "parachute_mass_props.h"
#include "parachute_pre_atmos.hh"
#include "parachute_eom.h"
#include "parachute_line_sail.h"
#include "parachute_twist_torque.h"
#include "parachute_relstate.hh"
#include "parachute_att_pnt_damping.hh"                      
#include "cml/models/interactions/subsonic_wake/include/wake_effects.hh"
#include "cml/models/dynamics/mass/mass_body_dispersed_init/include/mass_body_dispersed_init.hh"

#ifndef _PARACHUTE_EXEC_
#define _PARACHUTE_EXEC_

struct CLUSTER 
{
     double   inertial_pos[3];   /* (m) Parachute cluster inertial position */
     double   inertial_vel[3];   /* (m/s) Parachute cluster inertial velocity */
     double   chute_area;        /* (m2) Parachute cluster total chute drag area */
};

struct PARACHUTE_PARAMS 
{
     bool            hifi_model_active_flag;             /* (--) Model active flag */
     bool            has_confl_point;                    /* (--) Flag to indicate if chute cluster uses a confluence point */
     bool            model_confl_point;                  /* (--) Model confluence point flag */
     bool            line_twist_enabled;                 /* (--) Riser twist enabled flag */
     bool            fail_random_chute;                  /* (--) Fail a random chute flag */
     bool            fail_random_mortar;                 /* (--) Fail a random mortar flag */
     int             failed_chute;                       /* (--) Which chute in a cluster has been failed */
     int             skipped_chute;                      /* (--) WHich chute in a cluster to skip a reef stage */
     double          confpt_mass;                        /* (kg) Confluence point mass */
     double          min_chute_area;                     /* (m2) Minimun chute area to use in calculations */
     double          min_chute_CBAR;                     /* (m)  Minimum mean chord length to use in calculations */
     double          min_pilot_area;                     /* (m2) Minimum chute area of pilot to use in calculations */
     double          min_pilot_CBAR;                     /* (m)  Minimum pilot mean chord length to use in calculations */
     int             strap_counter;                      /* (--) Counter for main bag extraction */
     int             num_harness_lines;                  /* (--) Number of harness line to be modeled */
     int             num_pyld_attach_points;             /* (--) Number of chute attach points on payload */
     int             num_chutes;                         /* (--) Number of chutes in cluster */
     double          riser_line_length;                  /* (m)  Chute riser line length */
     double          suspension_line_length;             /* (m)  Chute suspension line length */
     double          harness_line_length[MAX_ITEMS::MAX_HLINES];    /* (m)  Harness line length */
     double          pilot_riser_length;                 /* (m)  Pilot chute riser line length */
     double          pilot_suspension_length;            /* (m)  Pilot chute suspension line length */
     double          main_riser_length;                  /* (m)  Main chute riser line length */
     double          main_suspension_length;             /* (m)  Main chute suspension line length */
     int             chute_att_pnt_index[MAX_ITEMS::MAX_CHUTES];    /* (--) Chute attach point index */
     int             harness_att_pnt_index[MAX_ITEMS::MAX_HLINES];  /* (--) Harness attach point index */
     double          bag_strap_strength;                 /* (N)  Main bag strap strength */
     double          pilot_diameter;                     /* (m)  Pilot chute full open diameter */
     double          main_diameter;                      /* (m)  Main chute full open diameter */
     double          zero_vector[3];                     /* (--) Vector populated with zeros */
     double          T_pyldSR_to_chuteSR[3][3];          /* (--) Transformation matrix of Payload SR to chute SR frames */
     double          confluence_pt_pos_SR[3];            /* (m)  Initial Confluence point position in SR frame */
     double          chute_pos_SR[MAX_ITEMS::MAX_CHUTES][3];        /* (m)  Initial chute position in SR frame */
     double          pilot_chute_pos_SR[MAX_ITEMS::MAX_CHUTES][3];  /* (m)  Initial pilot chute position in SR frame */
     double          T_identity[3][3];                   /* (--) Identity transformation matrix */
     bool            apd_damping;                        /* (--) Attach point damping flag */

    //New Vars to reset pyld att pnts for MM14 BM Runs
    bool            reset_att_pnts_enabled;                         /* (--) Flag to reset attach points if desired */
    double          new_pyld_att_pnt_sr[MAX_ITEMS::MAX_CHUTES][3];  /* (m)  Updated chute attach points */

    double          canopy_mass_ratio;                   /* (--)  Canopy mass ratio wrt to total chute mass */
    double          riser_mass_ratio;                    /* (--)  Riser line mass ratio wrt to total chute mass */
    double          suspension_line_mass_ratio;          /* (--)  Suspension line mass ratio wrt to total chute mass */
    double          plt_canopy_mass_ratio;               /* (--)  Pilot canopy mass ratio wrt to total chute mass */
    double          plt_riser_mass_ratio;                /* (--)  Pilot riser line mass ratio wrt to total chute mass */
    double          plt_suspension_line_mass_ratio;      /* (--)  Pilot suspension line mass ratio wrt to total chute mass */

    bool            drogue_deployed_mains;               /* (--)  Flag to indicate mains are deployed using drogue chutes */
    bool            pilot_chute_active;                  /* (--)  Flag to indicate pilot chute is part of current chute system */
    bool            use_lagged_atmos;                    /* (--)  Flag to use lagged payload atmosphere data */

    double          T_SR_to_body[3][3];                  /* (--) Chute SR to body frame transformation matrix */
    double          T_body_to_SR[3][3];                  /* (--) Chute body to SR frame transformation matrix */
    double          T_aero_to_body[3][3];                /* (--) Chute aero to body frame transformation matrix */
    double          LRS_origin_in_SR_frame[3];           /* (m)  Origin of LRS reference frame in CM SR frame */
    double          LRS_to_SR_rotation[3][3];            /* (--) Rotation matrix between LRS and SR frames */

};

struct PARACHUTE_OUTPUT
{
     double          force_on_payload[3];                /* (N)   Total chute force applied to payload */
     double          moment_on_payload[3];               /* (N*m) Total chute moment applied to payload */
     double          total_mortar_force[3];              /* (N)   Total chute mortar force applied to payload */
     double          total_mortar_moment[3];             /* (N*m) Total chute mortar moment applied to payload */
};


struct CONFLUENCE_DAMPING
{
    double damping_coefficient;    /* (N*s/m) Confluence point damping coefficient */
    double damping_force_inert[3]; /* (N) Confluenece point damping force */
};

struct CHUTE_INPUT
{
   double cg[3];                             /* (m) Payload cg */
   double pyld_mass;                         /* (kg) Payload mass */
   double gravaccel[3];                      /* (m/s2) gravity acceleration vector, inertial frame */
   double eci2ecef[3][3];                    /* (--) Earth Centered Inertial to Earth Centered Earth Fixed Transformation Matrix */
   double atm_density;                       /* (kg/m3) Atmospheric density */
   double atm_sos;                           /* (m/s) Current speed of sound */
   double atm_wind_vel_NED[3];               /* (m/s) Wind velocity in NED Frame */
   double pyld_geocen_alt;                   /* (m) Payload geocentric altitude */
   double pyld_geocen_lat;                   /* (rad) Payload geocentric latitude */
   double pyld_geocen_lon;                   /* (rad) Payload geocentric longitude */
   double pyld_Vmag_earthrel;                /* (m/s) Payload earth relative velocity magnitude */
   double pyld_ecef_pos[3];                  /* (m) Payload Earth Centered Earth Fixed position */
   double pyld_inertial_pos[3];              /* (m) Payload inertial position */
   double pyld_inertial_vel[3];              /* (m/s) Payload inerital velocity */
   double pyld_inertial_rate[3];             /* (rad/s) Payload body rate in inertial frame */
   double pyld_T_inertial_to_body[3][3];     /* (--) Payload inertial to body transformation matrix */
   double pyld_T_body_to_SR[3][3];           /* (--) Payload body to SR transformation matrix */
   double pyld_angle_of_attack_in;           /* (rad) Payload angle of attack */
   double pyld_angle_of_attack_total_in;     /* (rad) Payload total angle of attack */
   double pyld_sideslip_in;                  /* (rad) Payload sideslip angle */
   double pyld_qbar;                         /* (N/m2) Payload dynamic pressure */
   double pyld_VairB[3];                     /* (m/s) Payload velocity vector in body frame */
   double parent_inertial_pos[3];            /* (m) Parent body of composite body inertial position */
   double parent_T_inertial_to_body[3][3];   /* (--) Parent body of composite body inertial to body transformation matrix */
   double pyld_Q_inertial_to_body_scalar;    /* (--) Payload Quarternion scalar */
   double pyld_Q_inertial_to_body_vector[3]; /* (--) Payload Quarternion vector */

   double EI_pyld_inertial_pos[3];           /* (m ) Payload inertial position */
   double EI_pyld_inertial_vel[3];           /* (m/s) Payload inertial velocity */
   double EI_pyld_inertial_rate[3];          /* (rad/s) Payload inertial rate */
   double EI_pyld_T_inertial_to_body[3][3];  /* (--) Payload inertial to body transformation matrix */
   double EI_pyld_T_body_to_SR[3][3];        /* (--) Payload body to SR transformation matrix */
   double EI_cg[3];                          /* (m)  Payload cg */
   double EI_pyld_Q_inertial_to_body_scalar; /* (--) Payload Quarternion scalar */
   double EI_pyld_Q_inertial_to_body_vector[3];  /* (--) Payload Quarternion vector */

};

//struct ParachuteEnums
//{
//     enum ChutePhase {
//               mortar_fire       = 0,
//               pilot_free        = 1,
//               main_deployment   = 2,
//               main_inflating    = 3,
//               main_steady_state = 4};
//
//     enum MainDeploySubphase {
//               pilot_inflating    = 0,
//               main_free          = 1,
//               bag_strip          = 2};
//};


struct PARACHUTE_DATA 
{
     enum chute_phases {
               mortar_fire       = 0,
               pilot_free        = 1,
               main_deployment   = 2,
               main_inflating    = 3,
               main_steady_state = 4};

     enum main_deploy_subphases {pilot_inflating    = 0, 
                                 main_free          = 1, 
                                 bag_strip          = 2};

     int                    chutes_deployed;                               /* (--) 1 for chutes deploy trigger (start model sequence) */
     PARACHUTE_DATA::chute_phases           chute_phase[MAX_ITEMS::MAX_CHUTES];            /* (--) Current chute operational phase */
     PARACHUTE_DATA::main_deploy_subphases  main_deploy_subphase[MAX_ITEMS::MAX_CHUTES];   /* (--) Main deploy subphase */

     bool            bag_strip_first_pass_complete[MAX_ITEMS::MAX_CHUTES]; /* (--) Bag strip first pass complete flag */
     bool            pilot_main_system;                                    /* (--) Flag to inform model to compute state between pilot chute and main bag */
     bool            main_bag_release[MAX_ITEMS::MAX_CHUTES];              /* (--) Main bag released from bay flag */
     bool            all_chutes_released;                                  /* (--) All chutes released from payload */
     bool            chute_released[MAX_ITEMS::MAX_CHUTES];                /* (--) Flag to indicate if specific chute has been released from payload */
     double          sim_time_local;                                       /* (s)  Current sim time */
     int             mortar_fired[MAX_ITEMS::MAX_CHUTES];                  /* (--) Mortar fired flag */
     double          dt;                                                   /* (s)  Chute model execution rate */

     double          T_ECEF_topd[3][3];                                    /* (--) Chute Earth Centered Earth Fixed to Topodetic transformation matrix */
     double          wind_vel_inertial[3];                                 /* (m/s) Chute wind velocity in inertial frame */
     double          atm_wind_vel_pfix[3];                                 /* (m/s) Chute wind velocity in planet fixed frame */

     //Variables for drogue deployed mains option
     double temp_T_b_to_i[3][3];                                           /* (--) Copy of drogue body to inertial transformation matrix*/
     double temp_T_i_to_b[3][3];                                           /* (--) Copy of drogue inertial to body transformation matrix */
     double drogue_force[3];                                               /* (N)  Drogue force vector to apply to main bag if modeling drogue deployed mains */

     // Chute Init Vars
     bool            conflpnt_init;                                        /* (--)  Flag to tell code to initialize confluence point state */
     bool            conflpnt_init_complete;                               /* (--)  Flag that indicates confluence point initialziation is complete */
     bool            parachute_init[MAX_ITEMS::MAX_CHUTES];                /* (--)  Flag to tell code to initialize chute state */
     bool            init_complete[MAX_ITEMS::MAX_CHUTES];                 /* (--)  Flag that indicates chute initialization is complete */
 
     //Chute Parameters (constants)
     PARACHUTE_PARAMS param;                                               /* (--)  Instance of Params data structure */
 
     //Chute Output
     PARACHUTE_OUTPUT output;                                              /* (--)  Instance of Output data structure */

     //Mortar Model
     MORTAR_DATA    mortar[MAX_ITEMS::MAX_MORTARS];                        /* (--)  Instance of Mortar data structure */

     // Inflation Model
     INFLATION_DATA    pilot_inflation[MAX_ITEMS::MAX_CHUTES];               /* (--)  Instance of Inflation data structure for pilots */
     INFLATION_DATA    main_inflation[MAX_ITEMS::MAX_CHUTES];                /* (--)  Instance of Inflation data structure for main */
     INFLATION_DATA    main_inflation_skip_data[MAX_ITEMS::MAX_CHUTES];      /* (--)  Instance of Inflation data structure for chute that skips a reef stage */
     INFLATION_DATA    main_inflation_lag_data[MAX_ITEMS::MAX_CHUTES];       /* (--)  Instance of Inflation data structure for lagging chutes that don't skip a reef stage */

     // Aero Model
     AERO_DATA    pilot_aero[MAX_ITEMS::MAX_CHUTES];                         /* (--)  Instance of Aero data structure for pilots */
     AERO_DATA    main_aero[MAX_ITEMS::MAX_CHUTES];                          /* (--)  Instance of Aero data structure for mains */
     AERO_DATA    confpt_aero;                                               /* (--)  Instance of Aero data structure for confluence point */

     // Coupling Model
     CHUTE_COUPLING_DATA   pilot_coupling[MAX_ITEMS::MAX_CHUTES];            /* (--)  Instance of Coupling data structure for pilots */
     CHUTE_COUPLING_DATA   main_coupling[MAX_ITEMS::MAX_CHUTES];             /* (--)  Instance of Coupling data structure for mains */
     CHUTE_COUPLING_DATA   harness_coupling[MAX_ITEMS::MAX_ATTACH_PTS];      /* (--)  Instance of Coupling data structure for confluence point */

     // Attach Point Damping Model
     ATT_PNT_DATA apd[MAX_ITEMS::MAX_CHUTES];                              /* (--)  Instance of attach point damping data structure */

     //Mass Prop Model
     MASS_DATA pilot_massprops[MAX_ITEMS::MAX_CHUTES];                     /* (--)  Instance of chute mass props data structure for pilots */
     MASS_DATA main_massprops[MAX_ITEMS::MAX_CHUTES];                      /* (--)  Instance of chute mass props data structure for mains */

     //GRAM Interface
     STATE_DATA gram;                                                      /* (--)  Instance of atmosphere state data structure */
 
     //EOM
     EOM_DATA   eom_confpt;                                                  /* (--)  Instance of equations of motion data structure for confluence point */
     EOM_DATA   eom_pilot[MAX_ITEMS::MAX_CHUTES];                            /* (--)  Instance of equations of motion data structure for pilots */
     EOM_DATA   eom_main[MAX_ITEMS::MAX_CHUTES];                             /* (--)  Instance of equations of motion data structure for mains */

     //INPUT
     CHUTE_INPUT input;                                                    /* (--)  Instance of chute input data structure */

     //TWIST
     TWIST_DATA twist;                                                     /* (--)  Instance of chute twist torque data structure */

     //REL STATE
     RELSTATE_DATA relstate;                                               /* (--)  Instance of chute relstate data structure */
     RELSTATE_DATA pilot_main_relstate;                                    /* (--)  Instance of chute relstate data structure */

     //DEPLOY 
     int bag_strap_counter[MAX_ITEMS::MAX_CHUTES];                         /* (--)  Bag strap counter, to determine when main bag is extracted from bay */

     //LINE SAIL
     LINE_SAIL_DATA line_sail[MAX_ITEMS::MAX_CHUTES];                      /* (--) Instance of line sail data structure */

     // Subsonic Wake Effects Model
     WakeEffectsOut  wake_effects;                                         /* (--)  Instance of wake effects data structure */

     // Confluence damping
     CONFLUENCE_DAMPING conf_damping;                                      /* (--)  Instance of confluence damping data structure */

     // Chute Cluster Paramaters
     CLUSTER cluster;                                                      /* (--)  Instance of cluster data structure */

     //ANALYSIS
     double total_load;                                             /* (N)  Total Riser Line Load of All Chutes              */
     double peak_reef1_max_load[MAX_ITEMS::MAX_CHUTES];             /* (N)  Peak Reef1 Riser Line Load of Each Chute         */
     double qbar_at_peak_reef1_max_load[MAX_ITEMS::MAX_CHUTES];     /* (N/m2)  Peak Reef1 Riser Line Load of Each Chute      */
     double peak_reef2_max_load[MAX_ITEMS::MAX_CHUTES];             /* (N)  Peak Reef2 Riser Line Load of Each Chute         */
     double qbar_at_peak_reef2_max_load[MAX_ITEMS::MAX_CHUTES];     /* (N/m2)  Peak Reef1 Riser Line Load of Each Chute      */
     double peak_full_max_load[MAX_ITEMS::MAX_CHUTES];              /* (N)  Peak Disreef Riser Line Load of Each Chute       */
     double qbar_at_full_max_load[MAX_ITEMS::MAX_CHUTES];           /* (N/m2)  Peak Reef1 Riser Line Load of Each Chute      */
     double deploy_start_time[MAX_ITEMS::MAX_CHUTES];               /* (s)  Sim Start Time of Chute Deployment               */
     double deploy_end_time[MAX_ITEMS::MAX_CHUTES];                 /* (s)  Sim Stop Time of Chute Deployment                */
     double deploy_time[MAX_ITEMS::MAX_CHUTES];                     /* (s)  Deploy Time for Each Chute                       */
     double peak_full_load_vector[MAX_ITEMS::MAX_CHUTES][3];        /* (N)  Vector of peak load                              */

     //New vars for pilot modeling
     double temp_distance[3];                            /* (m) Distance of pilot chute to use after main line stretch */
     double pilot_peak_load[3];                          /* (N) Var to track peak pilot riser load */
     double qbar_at_pilot_peak_load[3];                  /* (N/m2) Var to track qbar at peak pilot riser load */

     // Load Sharing Var
     double load_share[MAX_ITEMS::MAX_CHUTES];           /* (--) Load Sharing amongst chutes */

     // Var to determine when to turn on twist torque model, currently starting when all mains have deployed
     int turn_twist_on_check;                           /* (--) Counter to see if all mains have deployed */

     // Vars to capture peak cluster load data
     double total_payload_force_mag;           /* (N) Chute data for performance monitoring */
     double peak_cluster_load_mag;             /* (N) Chute data for performance monitoring */
     double peak_reef1_cluster_load_mag;       /* (N) Chute data for performance monitoring */
     double peak_reef1_cluster_load_vec[3];    /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_x;          /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_x;          /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_x_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_x_vec[3];   /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_y;          /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_y;          /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_y_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_y_vec[3];   /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_z;          /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_z;          /* (N) Chute data for performance monitoring */
     double min_reef1_cluster_load_z_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef1_cluster_load_z_vec[3];   /* (N) Chute data for performance monitoring */
     double peak_reef2_cluster_load_mag;       /* (N) Chute data for performance monitoring */
     double peak_reef2_cluster_load_vec[3];    /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_x;          /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_x;          /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_x_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_x_vec[3];   /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_y;          /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_y;          /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_y_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_y_vec[3];   /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_z;          /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_z;          /* (N) Chute data for performance monitoring */
     double min_reef2_cluster_load_z_vec[3];   /* (N) Chute data for performance monitoring */
     double max_reef2_cluster_load_z_vec[3];   /* (N) Chute data for performance monitoring */
     double peak_disreef_cluster_load_mag;     /* (N) Chute data for performance monitoring */
     double peak_disreef_cluster_load_vec[3];  /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_x;        /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_x;        /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_x_vec[3]; /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_x_vec[3]; /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_y;        /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_y;        /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_y_vec[3]; /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_y_vec[3]; /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_z;        /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_z;        /* (N) Chute data for performance monitoring */
     double min_disreef_cluster_load_z_vec[3]; /* (N) Chute data for performance monitoring */
     double max_disreef_cluster_load_z_vec[3]; /* (N) Chute data for performance monitoring */
     double current_load_mag[3];               /* (N) Chute data for performance monitoring */
     double peak_load_mag[3];                  /* (N) Chute data for performance monitoring */
     double qbar_at_peak_load[3];              /* (N/m2) Chute data for performance monitoring */
     double qbar_at_peak_cluster_load;         /* (N/m2) Chute data for performance monitoring */
     double qbar_at_peak_reef1_cluster_load;   /* (N/m2) Chute data for performance monitoring */
     double qbar_at_peak_reef2_cluster_load;   /* (N/m2) Chute data for performance monitoring */
     double qbar_at_peak_disreef_cluster_load; /* (N/m2) Chute data for performance monitoring */

     //Vars to track Min/Max Y&Z 45 Deg Fairlead Loads
     double T_45Deg_Fairlead[3][3];            /* (--) Chute data for performance monitoring */
     double force_prime[3];                    /* (N) Chute data for performance monitoring */
     double max_y_force_prime[3];              /* (N) Chute data for performance monitoring */
     double min_y_force_prime[3];              /* (N) Chute data for performance monitoring */
     double max_z_force_prime[3];              /* (N) Chute data for performance monitoring */
     double min_z_force_prime[3];              /* (N) Chute data for performance monitoring */
     double max_y_fp_CM[3];                    /* (N) Chute data for performance monitoring */
     double min_y_fp_CM[3];                    /* (N) Chute data for performance monitoring */
     double max_z_fp_CM[3];                    /* (N) Chute data for performance monitoring */
     double min_z_fp_CM[3];                    /* (N) Chute data for performance monitoring */
     double min_reef1_y_force_prime;           /* (N) Chute data for performance monitoring */
     double max_reef1_y_force_prime;           /* (N) Chute data for performance monitoring */
     double min_reef1_z_force_prime;           /* (N) Chute data for performance monitoring */
     double max_reef1_z_force_prime;           /* (N) Chute data for performance monitoring */
     double min_reef2_y_force_prime;           /* (N) Chute data for performance monitoring */
     double max_reef2_y_force_prime;           /* (N) Chute data for performance monitoring */
     double min_reef2_z_force_prime;           /* (N) Chute data for performance monitoring */
     double max_reef2_z_force_prime;           /* (N) Chute data for performance monitoring */
     double min_disreef_y_force_prime;         /* (N) Chute data for performance monitoring */
     double max_disreef_y_force_prime;         /* (N) Chute data for performance monitoring */
     double min_disreef_z_force_prime;         /* (N) Chute data for performance monitoring */
     double max_disreef_z_force_prime;         /* (N) Chute data for performance monitoring */
     double min_reef1_y_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double max_reef1_y_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double min_reef1_z_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double max_reef1_z_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double min_reef2_y_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double max_reef2_y_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double min_reef2_z_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double max_reef2_z_force_prime_vec[3];    /* (N) Chute data for performance monitoring */ 
     double min_disreef_y_force_prime_vec[3];  /* (N) Chute data for performance monitoring */ 
     double max_disreef_y_force_prime_vec[3];  /* (N) Chute data for performance monitoring */ 
     double min_disreef_z_force_prime_vec[3];  /* (N) Chute data for performance monitoring */ 
     double max_disreef_z_force_prime_vec[3];  /* (N) Chute data for performance monitoring */ 

     //Vars to track Min/Max X&Z 45 Deg Fairlead Loads
     double T_45Deg_xz_Fairlead[3][3];         /* (--) Chute data for performance monitoring */
     double force_xz_prime[3];                 /* (N) Chute data for performance monitoring */
     double max_x_force_xz_prime[3];           /* (N) Chute data for performance monitoring */
     double min_x_force_xz_prime[3];           /* (N) Chute data for performance monitoring */
     double max_z_force_xz_prime[3];           /* (N) Chute data for performance monitoring */
     double min_z_force_xz_prime[3];           /* (N) Chute data for performance monitoring */
     double max_x_fp_xz_CM[3];                 /* (N) Chute data for performance monitoring */
     double min_x_fp_xz_CM[3];                 /* (N) Chute data for performance monitoring */
     double max_z_fp_xz_CM[3];                 /* (N) Chute data for performance monitoring */
     double min_z_fp_xz_CM[3];                 /* (N) Chute data for performance monitoring */

     //Vars to monitor alpha/beta 
     double min_alpha_total;                   /* (degree) Chute data for performance monitoring */
     double alpha_0to360;                      /* (degree) Chute data for performance monitoring */
     double min_alpha;                         /* (degree) Chute data for performance monitoring */
     double max_alpha;                         /* (degree) Chute data for performance monitoring */
     double min_alphas_beta;                   /* (degree) Chute data for performance monitoring */
     double max_alphas_beta;                   /* (degree) Chute data for performance monitoring */
     double min_beta;                          /* (degree) Chute data for performance monitoring */
     double max_beta;                          /* (degree) Chute data for performance monitoring */
     double min_betas_alpha;                   /* (degree) Chute data for performance monitoring */
     double max_betas_alpha;                   /* (degree) Chute data for performance monitoring */

     //Vars to monitor for riser/CM recontact
     double recontact_force[MAX_ITEMS::MAX_CHUTES][3];               /* (N)  Chute data for performance monitoring */
     double T_DSS_SR_to_Fairlead[3][3];                              /* (--) Chute data for performance monitoring */
     double chute_force_in_Fairlead_Frame[MAX_ITEMS::MAX_CHUTES][3]; /* (N)  Chute data for performance monitoring */
     double xy_angle[MAX_ITEMS::MAX_CHUTES];                         /* (rad)  Chute data for performance monitoring */
     double xz_angle[MAX_ITEMS::MAX_CHUTES];                         /* (rad)  Chute data for performance monitoring */
     double xy_angle_left_limit;                                     /* (rad)  Chute data for performance monitoring */
     double xy_angle_right_limit;                                    /* (rad)  Chute data for performance monitoring */
     double xy_angle_left_violation[MAX_ITEMS::MAX_CHUTES];          /* (--) Chute data for performance monitoring */
     double xy_angle_right_violation[MAX_ITEMS::MAX_CHUTES];         /* (--) Chute data for performance monitoring */
     double xz_angle_lower_limit;                                    /* (rad)  Chute data for performance monitoring */
     double xz_angle_upper_limit;                                    /* (rad)  Chute data for performance monitoring */
     double xz_angle_lower_violation[MAX_ITEMS::MAX_CHUTES];         /* (--) Chute data for performance monitoring */
     double xz_angle_upper_violation[MAX_ITEMS::MAX_CHUTES];         /* (--) Chute data for performance monitoring */
     double extreme_xy_angle_left[MAX_ITEMS::MAX_CHUTES];            /* (--) Chute data for performance monitoring */
     double extreme_xy_angle_right[MAX_ITEMS::MAX_CHUTES];           /* (--) Chute data for performance monitoring */
     double extreme_xz_angle_lower[MAX_ITEMS::MAX_CHUTES];           /* (--) Chute data for performance monitoring */
     double extreme_xz_angle_upper[MAX_ITEMS::MAX_CHUTES];           /* (--) Chute data for performance monitoring */

     //Skipped Stage Variables
     bool   skip_stage1;                                    /* (--)  Flag to skip stage1 inflation */
     bool   skip_stage2;                                    /* (--)  Flag to skip stage2 inflation */
     bool   skip_stage1_reset;                              /* (--)  Flag that indicates skip stage1 setup complete */
     bool   skip_stage2_reset;                              /* (--)  Flag that indicates skip stage2 setup complete */

     //All Chutes Inflated bool
     bool all_chutes_disreefed;                             /* (--)  Flag that indicates all chutes in cluster have fully inflated */

     //Vars to reset confluence point velocity at harness line stretch
     bool confpt_vel_reset;                                 /* (--)  Flag to reset confluence point velocity */
     bool harness_line_tight;                               /* (--)  Flag to indicate that at least one harness line is taut */

};

/* ------------------- */
/* Function Prototypes */
/* ------------------- */
void parachute_twist_exec     (PARACHUTE_DATA& chute); 

void parachute_phasing        (PARACHUTE_DATA& chute); 

void parachute_loads          (PARACHUTE_DATA& chute); 

void parachute_state          (PARACHUTE_DATA& chute); 

void parachute_attitude_reset (PARACHUTE_DATA& chute, 
                               EOM_DATA  &       chute_eom);

void parachute_mass_init      (const bool                   plt_chute, 
                               const MassBodyDispersedInit *chute_mass_props, 
                               MASS_DATA                   *chute_mass, 
                               PARACHUTE_DATA&              chute);

void parachute_exec_init      (const int        mortar_fired,
                               PARACHUTE_DATA&  chute);

int  parachute_exec           (const int        chute_cut,    
                               PARACHUTE_DATA&  chute, 
                               PARACHUTE_DATA&  droguechute);

void parachute_recontact_monitor(double PyldState_alpha_total,
                                 double PyldState_alpha,
                                 double PyldState_beta,
                                 double PyldState_qbar,
                                 PARACHUTE_DATA *chute);

#endif
