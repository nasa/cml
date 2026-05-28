/*******************************************************************************
PURPOSE:
  (Header file for AeroExecutiveSimple C++ class)

LIBRARY DEPENDENCY:
  ((../src/aero_executive_simple.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (March 2015) (Antares) (initial version))
   ((Brent Caughron) (OSR) (Dec 2020) (Antares) (Code Review and IV&V)))
*******************************************************************************/

#ifndef CML_AERO_EXECUTIVE_SIMPLE_HH
#define CML_AERO_EXECUTIVE_SIMPLE_HH

#include "aero_executive_base.hh"

/*******************************************************************************
AeroExecutiveSimple
Purpose:(Provides a simple aerodynamic executive to use with the AeroExec.)
*******************************************************************************/
class AeroExecutiveSimple : public AeroExecutiveBase
{
public:
  enum DragType {
    ballistic_coeff = 0,  // Use Ballistic Coefficient

    drag_coeff      = 1,  // Use Coefficient of drag

    constant_mag    = 2,  // Use a vector of constant magnitude
    constant_vec    = 3   // Use a constant vector
  };

protected:
  /***************************************************************************/
  // Additional references to external models, used as inputs
  /***************************************************************************/
  const double & dynamic_pressure;     /* (N/m2) Dynamic pressure */
  const double (&T_inrtl_struc)[3][3]; /* (--)
                inertial-to-structure transformation matrix. */
  const double & mass;                 /* (kg)   Vehicle mass. */
  const double * const free_stream_vel_unit;/* (m/s)
                Vehicle velocity wrt free stream, expressed in inertial.
                3-vector, held here as a pointer because it is not universally
                necessary, and where it is necessary it requires compatibility
                with Vector3 methods. */


  /***************************************************************************/
  // internal values
  /***************************************************************************/
  DragType drag_type;  /* (--) Specifies which model to use. */

public:
  double BC;           /* (kg/m2) Ballistic Coefficient. */
  double CD;           /* (--)    Coefficient of drag. */
  double drag_mag;     /* (N)     Magnitude of drag force. */
  double bc_epsilon;   /* (1/m2)
          A test-value for the validity of BC, ensuring that
          (dyn-pressure / BC) < (1/bc_epsilon).
          It is effectively 1/(the largest acceleration for which the
          BC approximation is valid). Defaults to 1E-6; That makes a
          maximum acceleration of 1E6 m/s^2. Can be adjusted to accommodate
          extreme cases. */

public:
  AeroExecutiveSimple( AeroInterfaceOutput & output_ref,
                       const double & dynamic_pressure,
                       const double * const free_stream_vel_in,
                       const double (&T_inrtl_struc_in)[3][3],
                       const double & mass_in);
  virtual ~AeroExecutiveSimple() {};

  void set_drag_type(DragType drag_type);

  virtual void initialize();
  virtual void update() override;

private:
  // Make the copy constructor and assignment operator private
  // (and unimplemented) to avoid erroneous copies.
  AeroExecutiveSimple (const AeroExecutiveSimple &);
  AeroExecutiveSimple & operator = (const AeroExecutiveSimple &);
};
#endif
