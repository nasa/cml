/*******************************************************************************
PURPOSE:
  (Header file for AeroExecutiveTable C++ class.
   This class adds the table-lookup option to the AeroExec class,
   by utilizing the generic AeroTableSet class to provide aerodynamic
   tables
   This class may utilize any number of AeroTableSet instances, but only
   one of those instances may be used at any given time.  Each instance
   should represent a unique phase of flight.)

LIBRARY DEPENDENCY:
  ((../src/aero_executive_table.cc)
   (../src/aero_executive_table_disperse.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2016) (Antares) (new))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_EXECUTIVE_TABLE_HH
#define CML_AERO_EXECUTIVE_TABLE_HH

#include "aero_executive_base.hh"
#include "aero_table_set_base.hh"
#include "aero_environment.hh"
#include "aero_interface_output.hh"

/*******************************************************************************
AeroExecutiveTable
Purpose: (Provides a table-lookup based aerodynamic executive.)
*******************************************************************************/
class AeroExecutiveTable : public AeroExecutiveBase
{
protected:
  /***************************************************************************/
  // External (input) Values
  /***************************************************************************/
  const double (&T_struc_to_body)[3][3]; /* (--)
       Struc to body transform matrix. Set at construction time. */

  AeroEnvironment & environment;     /* (--)
       References to numerous environment variables from the Interface. */
  const bool & disable_aero_moments; /* (--)
       Reference to the Interface instance. */
  const bool & disable_aero_damping; /* (--)
       Reference to the Interface instance. */

  /***************************************************************************/
  // Internal Values computed internally
  /***************************************************************************/
  double T_body_to_aero_frame[3][3]; /* (--)
          Transformation matrix from the vehicle body frame (used to
          specify the body rates) and the aero-frame
          relative to which the coefficients are specified. */
  double cos_beta;   /* (--) cosine of beta angle. */
  double sin_beta;   /* (--) sine of beta angle. */
  double cos_alpha;  /* (--) cosine of alpha angle. */
  double sin_alpha;  /* (--) sine of alpha angle. */
  double cos_attack; /* (--) cosine of attack angle. */
  double sin_attack; /* (--) sine of attack angle. */
  double cos_roll;   /* (--) cosine of roll angle. */
  double sin_roll;   /* (--) sine of roll angle. */

  double moment_arm[3];  /* (m)
          Current moment arm from moment reference center (MRC) to CG. */


  /***************************************************************************/
  // Internal Values copied from current table
  /***************************************************************************/
  double T_struc_to_aero_frame[3][3]; /* (--)
          Transformation matrix from the vehicle structure frame (used to
          specify the cg-position and mrc-position) and the aero-frame
          relative to which the coefficients are specified.*/
  AeroTableSetBase::AeroDataTableType data_table_type; /* (--)
          Copy of the aero-data-table type from the selected AeroTableSet. */
  bool uncertainties_expressed_as_percent; /* (--)
          Uncertainties are typically expressed as either
          (m) +/- (n) or as
          (m) +/- (p%)
          This flag distinguishes the two.
          Note that if true, the values for uncertainties should still be
          expressed in decimal form (e.g. 0.1 for 10%).
          If true,  bias = coeff * uncertainty * random.
          If false, bias = uncertainty * random.
          Default:false. */
  bool uncertainties_are_variable; /* (--)
          Flag indicating whether uncertainties are intended to be variable.
          Typically this would be because the uncertainties are populated by
          the table lookup.
          Default: false.*/

  bool aero_damping_in_table; /* (--) Damping data is provided. */
  AeroTableSetBase::AeroDampingType aero_damping_on_diag_in_table;  /* (--)
          Damping data includes on-diagonal terms. */
  AeroTableSetBase::AeroDampingType aero_damping_off_diag_in_table; /* (--)
          Damping data includes off-diagonal terms. */


  /***************************************************************************/
  // Internal Values - table-set selection
  /***************************************************************************/
  AeroTableSetBase * current_table; /* (--)  The current set of table-data. */
  std::vector<AeroTableSetBase *> data_tables_vector; /* (--)
         All possible sets of table-data. */

  /***************************************************************************/
  // Internal Values - Safety
  /***************************************************************************/
  double threshold_min_free_stream_vel_mag; /* (m/s)
         The minimum value of free-stream velocity-magnitude for
         aero-damping algorithm to be valid.*/

public:
  /***************************************************************************/
  // Additional user-specified inputs
  /***************************************************************************/
  bool dispersion_active; /* (--)
       Turns the dispersions on and off. */
  AeroCoefficientsDisp  bias; /* (--)
       The bias values to add to the nominal coefficient values. */
  AeroCoefficientsDisp  uncertainty; /* (--)
       The uncertainty on each value. Note that if uncertainties are NOT a
       part of the lookup, these values are constant for any given table-set.
       They are copied in from the table-set when the table-set is configured.*/
  AeroCoefficientsDisp  random; /* (--)
       The random values used to modify the uncertainty values.
       These are constant for the duration of the simulation, even if the
       table-set changes. */
protected:
  AeroCoefficientsDisp  modified_uncertainty; /* (--)
       The product of uncertainty and random for each coefficient.
       Interim value for cases involving uncertainties expressed as
       a percentage. */

public:
  bool load_all_tables_at_init; /* (--)
       Flag to indicate whether all tables should be loaded and initialized
       before the sim begins. This is safer if trying to switch tables
       mid-sim, but can be time-consuming if there are multiple tables, from
       which only a subset will be used. */

  enum LOverVEnum {
    // Which scale factor to use in the in the non-dimensionalization of the
    // aero damping coefficients. This is reserved as an option for certain
    // tables, but the default is the typical setting.
    // WARNING: These enumerations are used as specific numbers, not just
    //          a list of options. Change only with extreme caution!!! */
   Lref_over_Vmag  = 1,   // Use "Lref/Vmag (DEFAULT)"
   Lref_over_2Vmag = 2    // Use "Lref/(2*Vmag)"
  };

  LOverVEnum l_over_v_scale; /* (--) Which scale factor to use. */

  double mrc_position[3];    /* (m)
        Current aerodynamic moment reference center in structural frame. */

  /***************************************************************************/
  // outputs, for logging
  /***************************************************************************/
  AeroCoefficientsTable coefficients; /* (--) The relevant coefficients. */

  // Methods:
  AeroExecutiveTable( AeroInterfaceOutput & output_ref,
                      AeroEnvironment & environment,
                      const bool   & disable_aero_moments_in,
                      const bool   & disable_aero_damping_in,
                      const double (&T_struc_to_body_in)[3][3]);

  virtual ~AeroExecutiveTable(){};

  void change_table(unsigned int new_ix);
  void change_table(std::string table_name);
  void change_table(AeroTableSetBase & new_table);
  void add_table(AeroTableSetBase * new_table);

  virtual void initialize() override;
  virtual void update() override;
protected:
  virtual void activate() override;
  void configure_new_table(AeroTableSetBase *);
  void trig_functions();
  void post_process_table_data();
  void aero_forces_moments();

  // found in aero_executive_table_disperse.cc:
  void process_prelim_uncertainties();
  void populate_uncertainties();
  void disperse_data();
  void modify_uncertainties();
  void modify_damping_uncertainties_on_diagonal();
  void modify_damping_uncertainties_off_diagonal();
  void scale_bias();
  void scale_damping_bias();
  void increment_coeffs_with_bias();
  void increment_damping_coeffs_with_bias();


private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroExecutiveTable (const AeroExecutiveTable &);
  AeroExecutiveTable & operator = (const AeroExecutiveTable &);
};
#endif
