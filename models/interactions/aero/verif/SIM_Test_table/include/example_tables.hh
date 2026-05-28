/*******************************TRICK * HEADER******************************
PURPOSE: (Provide example tables for testing purposes)

PROGRAMMERS:
  (((Gary Turner) (OSR) (date) (project) (comment)))
**********************************************************************/

#ifndef EXAMPLE_TABLES_FOR_TESTING
#define EXAMPLE_TABLES_FOR_TESTING

#include "cml/models/utilities/table_interp_cpp/include/table_independent_variable.hh"
#include "cml/models/utilities/table_interp_cpp/include/generic_multi_input_table.hh"
#include "cml/models/interactions/aero/include/aero_table_set.hh"
#include "cml/models/interactions/aero/include/aero_executive_table.hh"

/*****************************************************************************
SimpleAeroTableSet
Purpose:(Basic form for testing the 6 types of input sets.
         This uses only 1 independent variable for simplicity.)
Note: - AeroTableSet is a TableLookupSet because the typical configuration
        would use multiple input variables (see ExampleTable* for a more
        typical configuration).  Consequently, cannot use SimpleAeroTableLookup
        even though there is only 1 independent variable.)
*****************************************************************************/
class SimpleAeroTableSet : public AeroTableSet
{
 public:
  TableIndependentVariable  independent;
  GenericMultiInputTable    dependents; /* (--) dependent variables. */
  std::vector<double*>      dep_var_list;
  bool                      include_diagonal_damping_terms;
  bool                      include_off_diagonal_damping_terms;
  bool                      include_diagonal_damping_unc_terms;
  bool                      include_off_diagonal_damping_unc_terms;


  SimpleAeroTableSet( AeroExecutiveTable & executive,
                      const double & indep_in,
                      std::string name_in)
    :
    AeroTableSet( name_in, executive),
    independent( "indy",    indep_in),
    dependents(),
    include_diagonal_damping_terms(false),
    include_off_diagonal_damping_terms(false),
    include_diagonal_damping_unc_terms(true),
    include_off_diagonal_damping_unc_terms(true)
  {};
  virtual ~SimpleAeroTableSet(){};

  virtual void initialize() {
    if (include_diagonal_damping_terms) {
      dep_var_list.push_back( &coefficients.dCl_dp);
      dep_var_list.push_back( &coefficients.dCm_dq);
      dep_var_list.push_back( &coefficients.dCn_dr);
    }
    if (include_off_diagonal_damping_terms) {
      dep_var_list.push_back( &coefficients.dCl_dq);
      dep_var_list.push_back( &coefficients.dCl_dr);
      dep_var_list.push_back( &coefficients.dCm_dp);
      dep_var_list.push_back( &coefficients.dCm_dr);
      dep_var_list.push_back( &coefficients.dCn_dp);
      dep_var_list.push_back( &coefficients.dCn_dq);
    }
    if (include_diagonal_damping_terms &&
        include_diagonal_damping_unc_terms) {
      dep_var_list.push_back( &uncertainty_ref.dCl_dp);
      dep_var_list.push_back( &uncertainty_ref.dCm_dq);
      dep_var_list.push_back( &uncertainty_ref.dCn_dr);
    }
    if (include_off_diagonal_damping_terms &&
        include_off_diagonal_damping_unc_terms) {
      dep_var_list.push_back( &uncertainty_ref.dCl_dq);
      dep_var_list.push_back( &uncertainty_ref.dCl_dr);
      dep_var_list.push_back( &uncertainty_ref.dCm_dp);
      dep_var_list.push_back( &uncertainty_ref.dCm_dr);
      dep_var_list.push_back( &uncertainty_ref.dCn_dp);
      dep_var_list.push_back( &uncertainty_ref.dCn_dq);
    }

    if (include_diagonal_damping_terms || include_off_diagonal_damping_terms) {
      name.append("_damping");
      if (include_off_diagonal_damping_unc_terms) {
        name.append("_uncertainty");
      }
    }

    dependents.set_output(dep_var_list);

    Aref = 1.0;
    Lref = 1.0;
    mrc_position[0] = 1.0;
    mrc_position[1] = 0.0;
    mrc_position[2] = 0.0;

    // independent data
    // There is 1 independent variable, with tabulated values {0, 1, 2}
    std::vector<double> indy_data(3, 0.0);
    indy_data[1] = 1.0;
    indy_data[2] = 2.0;
    independent.load_data( indy_data);

    // dependent_data
    // dependent data values takes the form:
    //  {n.0, n.1, n.2}
    // where n is the index of the dependent variable.
    size_t num_dep = dep_var_list.size();
    std::vector<double> dep_data;
    for (size_t ii = 0; ii < num_dep; ++ii) {
      dep_data.push_back( ii + 0.0);
      dep_data.push_back( ii + 0.1);
      dep_data.push_back( ii + 0.2);
    }
    std::vector<size_t> dim_list(2);
    dim_list[0] = num_dep;
    dim_list[1] = 3;
    dependents.load_data( dep_data,
                          dim_list);
    add_table(dependents);
    add_independent_variable(independent);
    associate_table_and_independent();

    AeroTableSet::initialize();
  }

 private:
  SimpleAeroTableSet (const SimpleAeroTableSet &);
  SimpleAeroTableSet & operator= (const SimpleAeroTableSet &);
};

/*****************************************************************************
SimpleAeroTableSet_SYM_LDm
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: SYM_LDm)
*****************************************************************************/
class SimpleAeroTableSet_SYM_LDm : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_SYM_LDm( AeroExecutiveTable & executive,
                              const double & indep)
    :
    SimpleAeroTableSet( executive, indep, "SYM_LDm")
    {};
  virtual void initialize() {
    data_table_type = SYM_LDm;
    dep_var_list.push_back( &coefficients.CD);
    dep_var_list.push_back( &coefficients.CL_sym);
    dep_var_list.push_back( &coefficients.Cm_sym);

    uncertainty.CD     = 0.1;
    uncertainty.CL_sym = 0.2;
    uncertainty.Cm_sym = 0.3;

    SimpleAeroTableSet::initialize();
  }
};
/*****************************************************************************
SimpleAeroTableSet_SYM_ANm
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: SYM_ANm)
*****************************************************************************/
class SimpleAeroTableSet_SYM_ANm : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_SYM_ANm( AeroExecutiveTable & executive,
                              const double & indep)
    :
    SimpleAeroTableSet( executive, indep,  "SYM_ANm")
    {};
  virtual void initialize() {
    data_table_type = SYM_ANm;
    dep_var_list.push_back( &coefficients.CA);
    dep_var_list.push_back( &coefficients.CN_sym);
    dep_var_list.push_back( &coefficients.Cm_sym);

    uncertainty.CA     = 0.1;
    uncertainty.CN_sym = 0.2;
    uncertainty.Cm_sym = 0.3;

    SimpleAeroTableSet::initialize();
  }
};
/*****************************************************************************
SimpleAeroTableSet_XYZ
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: XYZ)
*****************************************************************************/
class SimpleAeroTableSet_XYZ : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_XYZ( AeroExecutiveTable & executive,
                              const double & indep)
    :

    SimpleAeroTableSet( executive, indep, "XYZ")
    {};
  virtual void initialize() {
    data_table_type = XYZ;
    dep_var_list.push_back( &coefficients.CX);
    dep_var_list.push_back( &coefficients.CY);
    dep_var_list.push_back( &coefficients.CZ);
    dep_var_list.push_back( &coefficients.Cl_mrc);
    dep_var_list.push_back( &coefficients.Cm_mrc);
    dep_var_list.push_back( &coefficients.Cn_mrc);

    uncertainty.CX = 0.1;
    uncertainty.CY = 0.2;
    uncertainty.CZ = 0.3;
    uncertainty.Cl = 0.4;
    uncertainty.Cm = 0.5;
    uncertainty.Cn = 0.6;

    SimpleAeroTableSet::initialize();
  }
 private:
  SimpleAeroTableSet_XYZ (const SimpleAeroTableSet_XYZ &);
  SimpleAeroTableSet_XYZ & operator= (const SimpleAeroTableSet_XYZ &);
};
/*****************************************************************************
SimpleAeroTableSet_AYN
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: AYN)
*****************************************************************************/
class SimpleAeroTableSet_AYN : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_AYN( AeroExecutiveTable & executive,
                              const double & indep)
    :
    SimpleAeroTableSet( executive, indep, "AYN")
    {};
  virtual void initialize() {
    data_table_type = AYN;
    dep_var_list.push_back( &coefficients.CA);
    dep_var_list.push_back( &coefficients.CY);
    dep_var_list.push_back( &coefficients.CN);
    dep_var_list.push_back( &coefficients.Cl_mrc);
    dep_var_list.push_back( &coefficients.Cm_mrc);
    dep_var_list.push_back( &coefficients.Cn_mrc);

    uncertainty.CA = 0.1;
    uncertainty.CY = 0.2;
    uncertainty.CN = 0.3;
    uncertainty.Cl = 0.4;
    uncertainty.Cm = 0.5;
    uncertainty.Cn = 0.6;

    SimpleAeroTableSet::initialize();
  }
};
/*****************************************************************************
SimpleAeroTableSet_AYN_unc
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: AYN_unc)
*****************************************************************************/
class SimpleAeroTableSet_AYN_unc : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_AYN_unc( AeroExecutiveTable & executive,
                              const double & indep)
    :
    SimpleAeroTableSet( executive, indep, "AYN_unc")
    {};
  virtual void initialize() {
    data_table_type = AYN_unc;
    dep_var_list.push_back( &coefficients.CA);
    dep_var_list.push_back( &coefficients.CY);
    dep_var_list.push_back( &coefficients.CN);
    dep_var_list.push_back( &coefficients.Cl_mrc);
    dep_var_list.push_back( &coefficients.Cm_mrc);
    dep_var_list.push_back( &coefficients.Cn_mrc);
    dep_var_list.push_back( &uncertainty_ref.CA);
    dep_var_list.push_back( &uncertainty_ref.CY);
    dep_var_list.push_back( &uncertainty_ref.CN);
    dep_var_list.push_back( &uncertainty_ref.Cl);
    dep_var_list.push_back( &uncertainty_ref.Cm);
    dep_var_list.push_back( &uncertainty_ref.Cn);
    SimpleAeroTableSet::initialize();
  }
};
/*****************************************************************************
SimpleAeroTableSet_DSL
Purpose:(Derivative of SimpleAeroTableSet used for testing the option: DSL)
*****************************************************************************/
class SimpleAeroTableSet_DSL : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_DSL( AeroExecutiveTable & executive,
                              const double & indep)
    :
    SimpleAeroTableSet( executive, indep, "DSL")
    {};
  virtual void initialize() {
    data_table_type = DSL;
    dep_var_list.push_back( &coefficients.CD);
    dep_var_list.push_back( &coefficients.CS);
    dep_var_list.push_back( &coefficients.CL);
    dep_var_list.push_back( &coefficients.Cl_mrc);
    dep_var_list.push_back( &coefficients.Cm_mrc);
    dep_var_list.push_back( &coefficients.Cn_mrc);

    uncertainty.CD = 0.1;
    uncertainty.CS = 0.2;
    uncertainty.CL = 0.3;
    uncertainty.Cl = 0.4;
    uncertainty.Cm = 0.5;
    uncertainty.Cn = 0.6;

    SimpleAeroTableSet::initialize();
  }
};

/*****************************************************************************
SimpleAeroTableSet_Incomplete
Purpose:(Derivative of SimpleAeroTableSet used for testing whether incomplete
         configurations will get caught.)
*****************************************************************************/
class SimpleAeroTableSet_Incomplete : public SimpleAeroTableSet
{
 public:
  SimpleAeroTableSet_Incomplete( AeroExecutiveTable & executive,
                                 const double & indep)
    :

    SimpleAeroTableSet( executive, indep, "Incomplete")
    {};
  virtual void initialize() {
    dep_var_list.push_back( &coefficients.CA);
    SimpleAeroTableSet::initialize();
  }
};
/*****************************************************************************
ExampleTableSet1
Purpose:(Provides data for a AYN table as a function of multiple variables,
         in this case, (mach, alpha, beta))
Note - this is more illustrative than necessary for testing; the transition
       from 1 independent to 3 independents is actually a function of the
       TableInterp model, not of this model.
*****************************************************************************/
class ExampleTableSet1 : public AeroTableSet
{
 public:
  TableIndependentVariable  mach; /* (--) Mach number */
  TableIndependentVariable  alpha;/* (degree) alpha angle */
  TableIndependentVariable  beta; /* (degree) beta angle */
  GenericMultiInputTable    dependents; /* (--) dependent variables. */

 protected:
  const static size_t num_dependents = 6; /* (count)
        Number of dependent variables*/
  const static size_t size_mach_array = 3; /* (count)
        Number of data elements used to specify mach domain*/
  const static size_t size_alpha_array = 4; /* (count)
        Number of data elements used to specify alpha domain*/
  const static size_t size_beta_array = 5; /* (count)
        Number of data elements used to specify beta domain*/
 public:
  ExampleTableSet1( AeroExecutiveTable & executive,
                    const double & mach_number,
                    const double & angle_of_attack,
                    const double & angle_of_sideslip)
    :
    AeroTableSet("example_table", executive),
    mach(  "mach",              mach_number),
    alpha( "angle of attack",   angle_of_attack),
    beta(  "angle of sideslip", angle_of_sideslip),
    dependents()
  {
    std::vector<double*> dep_var_list(num_dependents);
    size_t i =0;
    dep_var_list[i++] = &executive.coefficients.CA;
    dep_var_list[i++] = &executive.coefficients.CY;
    dep_var_list[i++] = &executive.coefficients.CN;
    dep_var_list[i++] = &executive.coefficients.Cl_mrc;
    dep_var_list[i++] = &executive.coefficients.Cm_mrc;
    dep_var_list[i++] = &executive.coefficients.Cn_mrc;
    dependents.set_output(dep_var_list);
  }

  virtual ~ExampleTableSet1(){};

  virtual void initialize() {
    // don't try re-loading the data, but do still need to pass re-inits
    // through to AeroTableSet
    if (initialized) {
      AeroTableSet::initialize();
      return;
    }

    data_table_type = AYN;
    uncertainties_expressed_as_percent = false;
    Aref = 1.0;
    Lref = 1.0;
    mrc_position[0] = 1.0;
    mrc_position[1] = 0.0;
    mrc_position[2] = 0.0;

    // independent data
    // IMPORTANT NOTE - the variables angle_of_attack and angle_of_sideslip
    //   use radians as native units.  The data tables are often provided in degrees.
    //   So it would normally be necessary to convert the degree tables to radians
    //   before loading: e.g.:
    //      double scratch_alpha[size_alpha_array] = {-30, 0.0, 15, 30};
    //      for (int ii = 0; ii < size_alpha_array; ii++) {
    //          scratch_alpha[ii] *= radians_per_degree;
    //   However, for this unit testing mechanism, I'm just going to input values
    //   in radians directly.
    double scratch_mach[size_mach_array] = { 4.0, 6.0, 8.0};
    double scratch_alpha[size_alpha_array] = {-1.0, 0.0, 0.5, 1.0};
    double scratch_beta[size_beta_array] = {-0.2, -0.1, 0.0, 0.1, 0.2};
    mach.load_data(scratch_mach, size_mach_array);
    alpha.load_data(scratch_alpha, size_alpha_array);
    beta.load_data(scratch_beta, size_beta_array);

    // dependent_data
    // dependent data values are all of the form "0.dmab" where:
    //    d is the dependent variable index d=0 => executive.coefficients.CA
    //                                      d=1 => executive.coefficients.CY
    //                                      etc.
    //    m is the mach number index m=0 => mach-number < 6
    //                               m=1 => 6<= mach_number < 8
    //                               etc.
    //   a is the angle-of-attack index a=0 => angle-of-attack < 0.0 rad
    //                                  a=1 => 0.0 <= angle-of-attack <= 0.5 rad
    //                                  etc.
    //   b is the sideslip-angle index following the same pattern.
    //   So a value of 0.2212 will be assigned to
    //       CN (d=2) when
    //          mach = 8.0 (m=2), alpha = 0.0 (a = 1), beta = 0.0 (b=2)
    size_t ix2_mult = size_beta_array;
    size_t ix3_mult = size_alpha_array * ix2_mult;
    size_t ix4_mult = size_mach_array * ix3_mult;
    std::vector<double> dep_data( num_dependents*ix4_mult);
    for (size_t ix4 = 0; ix4 < num_dependents; ix4++) {
      for ( size_t ix3 = 0; ix3 < size_mach_array; ix3++) {
        for ( size_t ix2 = 0; ix2 < size_alpha_array; ix2++) {
          for ( size_t ix1 = 0; ix1 < size_beta_array; ix1++) {
            size_t ix  = ix4 * ix4_mult + ix3 * ix3_mult + ix2 * ix2_mult + ix1;
            dep_data[ix] = ix4 * 0.1 +
                           ix3 * 0.01 +
                           ix2 * 0.001 +
                           ix1 * 0.0001;
          }
        }
      }
    }
    std::vector<size_t> dim_list(4);
    dim_list[0] = num_dependents;
    dim_list[1] = size_mach_array;
    dim_list[2] = size_alpha_array;
    dim_list[3] = size_beta_array;
    dependents.load_data( dep_data,
                          dim_list);
    add_table(dependents);
    add_independent_variable(mach);
    associate_table_and_independent();
    add_independent_variable(alpha);
    associate_table_and_independent();
    add_independent_variable(beta);
    associate_table_and_independent();

    AeroTableSet::initialize();
  }

 private:
  ExampleTableSet1 (const ExampleTableSet1 &);
  ExampleTableSet1 & operator= (const ExampleTableSet1 &);
};

#endif
