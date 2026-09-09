/*******************************************************************************
PURPOSE:
   (Define the OrbElemSubset class.
   (The OrbElemSubset model provides a subset of the orbital elements for
   support of non-orbital prediction methods such as impact-point and
   line-of-apsides predictors.
   This model is not intended for stand-alone usage, but as a utility for calling
   from stand-alone models in support of such.)

LIBRARY DEPENDENCIES:
   ((../src/orb_elem_subset.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (Oct 2014) (New implementation for Antares))
    ((Bingquan Wang) (OSR) (May 2017) (changed the way to declare LIBRARY DEPENDENCY 
                                       from .o to .cc))
   )
 ******************************************************************************/

#ifndef CML_ORB_ELEM_SUBSET_HH
#define CML_ORB_ELEM_SUBSET_HH

#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"

// Forward declaration
namespace jeod {
   class Planet;
}

class OrbElemSubset{

 public:
  enum OrbitType{
    UNDEFINED = -1,
    ELLIPSE = 0,
    HYPERBOLA = 1,
    PARABOLA = 2
  };
  // Flags
  bool use_pfix_z_values{false}; /* (--)
      The z-values used in computing sinI_sinL and sinI_cosL should be pfix.
      The legacy implementation indicates pfix but actually does not
      distinguish between inertial and pfix, so uses inertial.
      There is very little difference.  This flag states that the pfix values
      shall be used, and not the inertial values.  Defaults off. */

  // Inertial positions
  double position[3]{};       /* (m)
       The position vector on which the orb-elem are based. */
  double position_unit[3]{};  /* (--) unit vector */
  double position_mag{0.0};   /* (m)  magnitude of position vector */

  // Inertial velocities
  double velocity[3]{};     /* (m/s)
       The velocity vector on which the orb-elem are based. */
  double vel_vertical[3]{}; /* (m/s) component that represent vertical motion */
  double vel_horizontal_unit[3]{}; /* (--)  unit vector of horizontal component */
  double vel_vertical_scalar{0.0}; /* (m/s)
           magnitude of vertical with sign (+up / -down) */

  // Classical elements
  double semi_major_axis{0.0}; /* (m) a */
  double semi_parameter{0.0};  /* (m) p = h^2 / mu */
  double ecc_mag{0.0};         /* (--)  magnitude of the eccentricity vector */
  double ecc_anomaly{0.0};     /* (rad) Eccentric anomaly */
  double true_anomaly{0.0};    /* (rad) true anomaly */
  double arg_periapsis{0.0};   /* (rad) argument of periapsis */
  double mean_motion{0.0};     /* (rad/s) rate of advance of mean anomaly. */

  // Support values
  OrbitType orbit_type{UNDEFINED}; /* (--) type of orbit */
  double specific_energy{0.0};     /* (J/kg) Specific orbital energy */
  double tol_energy{1.0E-6};       /* (J/kg) Tolerance to check if spec. energy is zero */
  double ang_momentum[3]{};        /* (m2/s) Angular momentum vector (inertial) */
  double node_line[3]{};           /* (--) Vector along line of nodes */
  double cos_true_anomaly{0.0};    /* (--) cosine of the true anomaly */
  double node_angle{0.0};          /* (rad)
      angle from ascending node to current position.
      node_angle = arg_periapsis + true_anomaly.*/
  double sinI_sinL{0.0};           /* (--) sin inclination * sin(node_angle) */
  double sinI_cosL{0.0};           /* (--) sin inclination * cos(node_angle) */
  double sin_sq_I{0.0};            /* (--) sin inclination * sin inclination */
  double e_sinE{0.0};              /* (--) eccentricity * sin eccentric-anomaly */
  double e_cosE{0.0};              /* (--) eccentricity * cos eccentric-anomaly */

  // J2 corrections:
  double j2{0.0010836};            /* (--) The J2 term */
  double j2_r_eq_2{0.0};           /* (m2) J2 time square of equatorial radius */
  double j2_correction{0.0};       /* (m)  (j2_r_eq_2) / (4P), P = semiparameter */
  double j2_delta{0.0};            /* (m)  height adjustment */

 protected:
  const jeod::RefFrameTrans & inertial_state; /* (--)
       Reference to the translational state relative to the inertial frame.*/
  const jeod::Planet & planet; /* (--)
       Reference to the planet used as the basis for the orbital elements. */
  const double  & mu; /* (m3/s2) Gravitational parameter for "planet". */
  double polar_axis_inrtl[3]{}; /* (--)
      Vector describing the orientation of the polar axis, expressed in the
      inertial frame.*/

 public:
  void initialize(); 
  void update();
  void compute_eccentricities();
  void compute_true_anomaly();
  void compute_arg_periapsis();
  void compute_line_of_nodes();
  void generate_cartesian_inertial();
  void compute_mean_motion();
  void correct_for_j2();

  OrbElemSubset(const jeod::RefFrameTrans & inrtl_state_in,
                const jeod::Planet & planet_in,
                const double & planet_mu);
  ~OrbElemSubset() = default;
  OrbElemSubset(const OrbElemSubset& rhs) = delete;
  OrbElemSubset & operator = (const OrbElemSubset&) = delete;
};

#endif