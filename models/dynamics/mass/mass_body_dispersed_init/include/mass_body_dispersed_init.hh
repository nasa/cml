/*******************************************************************************
PURPOSE:
  (Define the class MassBodyDispersedInit, a class for initializing the core
   mass properties of a jeod::MassBody object to include dispersions.
   If the properties can just be dispersed across a known range, just use
   MassBodyInit. Use this class if the dispersion itself is variable.)

LIBRARY DEPENDENCIES:
  (../src/mass_body_dispersed_init.cc)

PROGRAMMERS:
  (((Gary Turner) (OSR) (August 2015) (Antares) (Initial version))
   ((Brent Caughron) (OSR) (July 2017) (Antares) (IV&V model code cleanup))
   ((Gary Turner) (OSR) (April 2018) (Antares) (Refactor))
   ((Brent Caughron) (OSR) (Mar 2021) (Antares) (Secondary IV&V)))
*******************************************************************************/
#ifndef CML_MASS_BODY_DISPERSED_INIT_HH
#define CML_MASS_BODY_DISPERSED_INIT_HH

// JEOD headers:
#include "jeod/models/dynamics/body_action/include/mass_body_init.hh"

// Local headers:
#include "mass_properties_dispersions.hh"

// Forward Declarations:
namespace jeod {
class DynManager;
}

/*******************************************************************************
The instantiable replacement for MassBodyInit. The apply(...) method in this
class can be used as a direct replacement for the
MassbodyInit::apply(...) method.
*******************************************************************************/
class MassBodyDispersedInit : public jeod::MassBodyInit,
                              public MassPropertiesDispersionsFlags {
public:
  MassPropertiesDispersions disperse; /* (--) The dispersed values. */
  MassBodyDispersedInit();
  virtual ~MassBodyDispersedInit(){};

  virtual void apply( jeod::DynManager & dyn_manager);

private: // and undefined:
  MassBodyDispersedInit (const MassBodyDispersedInit& rhs);
  MassBodyDispersedInit& operator = (const MassBodyDispersedInit& rhs);
};
#endif
