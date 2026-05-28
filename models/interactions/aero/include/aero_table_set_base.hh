/*******************************TRICK HEADER************************************
PURPOSE: (Standalone front-end to the aerodynamics data tables.
          The AeroTableSet is a specific case of the more generic
          TableLookupSet, with the intent specifically focused on
          data tables representing aerodynamics.
          The AeroTableSetBase provides the actual content,and is
          inherited into AeroTableSet to provide the interface with the
          AeroExecutiveTable.)

LIBRARY DEPENDENCY:
  ((../src/aero_table_set_base.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Dec 2015) (Antares) (initial version))
   ((Gary Turner) (OSR) (June 2016) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_TABLE_SET_BASE_HH
#define CML_AERO_TABLE_SET_BASE_HH
#include <string>
#include "cml/models/utilities/table_interp_cpp/include/table_lookup_set.hh" //TableLookupSet

#include "aero_coefficients.hh" // AeroCoefficientsTable

class AeroTableSetBase: public TableLookupSet
{
protected:
  /***************************************************************************/
  //  External values
  /***************************************************************************/
  AeroCoefficientsTable & coefficients; /* (--)
          Reference to the interface instance of aero-coefficients.
          NOTE - there is one interface per vehicle, but potentially
              many AeroTableSet instances.  Each AeroTableSet is responsible
              for populating the same set of coefficients.*/
  AeroCoefficientsDisp & uncertainty_ref; /* (--)
         Reference to the table-executive instance of aero-coefficients for
          dispersion (AeroExecutiveTable::uncertainty). */

public:
  /***************************************************************************/
  // Non-tabular data.  Tabular data is defined in the specific implementation
  // so that it can be sized accordingly.
  /***************************************************************************/
  std::string   name;     /* (--)
          Table name, useful for being able to switch between tables at
          flight-phase boundaries.*/
  double Aref; /* (m2)
          Locally-specific value of the reference-area
          Tables may be configured for a particular value, so this becomes a
          table-level setting that must be pushed up to the master interface.*/
  double Lref; /* (m)
          Locally-specific value of the reference-length
          Tables may be configured for a particular value, so this becomes a
          table-level setting that must be pushed up to the master interface.*/
  double T_struc_to_aero_frame[3][3]; /* (--)
          Transformation matrix from the vehicle structure frame (used to
          specify the cg-position and mrc-position) and the aero-frame
          relative to which the coefficients are specified.*/
  double mrc_position[3];       /* (m)
        Current aerodynamic moment reference center in structural frame*/



  bool uncertainties_expressed_as_percent;   /* (--)
        Uncertainties are typically expressed as either
        (m) +/- (n) or as
        (m) +/- (p%)
        This flag distinguishes the two.
        Note that if true, the values for uncertainties should still be
        expressed in decimal form (e.g. 0.1 for 10%).
        If true,  bias = coeff * uncertainty * random.
        If false, bias = uncertainty * random.
        Default:false. */
  AeroCoefficientsDisp  uncertainty; /* (--)
       The uncertainty on each value.  Note that if uncertainties are NOT a
       part of the lookup, these values are constant for any given table-set.
       In that case, they may be specified here.  They are copied out to
       table-executive AeroExecutiveTable::uncertainty when this table-set
       is configured for use by the executive.
       If uncertainties ARE a part of the lookup, these values will still
       be copied to the executive, but the table update will subsequently
       overwrite them.*/


  /***************************************************************************/
  //   Interpretation of table data:
  /***************************************************************************/
  enum AeroDataTableType
  {
    Unspecified = -1, // Default, causes termination.
    SYM_LDm = 0,      // Symmetric vehicle: CL, CD, and pitching moment coef
    SYM_ANm = 1,      // Symmetric vehicle: CA, CN, and pitching moment coef
    XYZ = 2,          // Input body force and body moment coefficients
    AYN = 3,          /* Input axial, side, and normal force coefficients and
                           body moment coefficients. */
    AYN_unc = 4,      /* Input axial, side, and normal force coefficients and
                           body moment coefficients with uncertainties. */
    DSL = 5           /* Input drag, side, and lift force coefficients and
                           body moment coefficients. */
  };

  enum AeroDampingType
  {
    NotInTable = 0, // Not included in table
    CoeffsOnly = 1, // Coefficients included, but not uncertainties
    CoeffsUnc  = 2  // Coefficients and their uncertainties included.
  };



protected:
  /***************************************************************************/
  // Internal variables
  /***************************************************************************/

  bool aero_damping_in_table; /* (--) Result of verif from configure_table. */
  AeroDataTableType data_table_type; /* (--)
          The type of data-table being used - identifies which variables are
          contained in the table. */
  AeroDampingType aero_damping_on_diag_in_table;  /* (--)
          The on-diagonal elements are set in the table. */
  AeroDampingType aero_damping_off_diag_in_table; /* (--)
          The on-diagonal elements are set in the table. */

  bool uncertainty_data_present; /* (--)
          Internal recognition that the tables include uncertainty data. */
  bool coef_data_present;        /* (--)
          Internal recognition that the tables include regular coeff data. */

public:
  AeroTableSetBase( const std::string & name_in,
                    AeroCoefficientsTable & coefficients_out,
                    AeroCoefficientsDisp  & uncertainties_out);

  virtual ~AeroTableSetBase() {};

  virtual void initialize();
  void configure_table();
  bool get_damping_in_table() { return aero_damping_in_table;};
  AeroDataTableType get_table_type() { return data_table_type;};
  AeroDampingType get_damping_on_diag_in_table() {
                         return aero_damping_on_diag_in_table;};
  AeroDampingType get_damping_off_diag_in_table() {
                         return aero_damping_off_diag_in_table;};

  void set_table_type(AeroDataTableType new_type);

protected:
  void query_aero_damping();
  void query_on_diag_aero_damping();
  void query_off_diag_aero_damping();
  AeroDampingType verify_aero_damping(std::string type);

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroTableSetBase (const AeroTableSetBase &);
  AeroTableSetBase & operator = (const AeroTableSetBase &);
};
#endif
