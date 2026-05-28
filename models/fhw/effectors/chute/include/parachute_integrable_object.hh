/*******************************************************************************
Purpose:
   (Allow a chute state to be integrated by JEOD.)

Library Dependency:
   ((parachute_integrable_object.o))
*******************************************************************************/

#ifndef PARACHUTE_INTEGRABLE_OBJECT_HH
#define PARACHUTE_INTEGRABLE_OBJECT_HH

#include "jeod/models/utils/integration/include/restartable_state_integrator.hh"
#include "er7_utils/integration/core/include/integrable_object.hh"
#include "er7_utils/integration/core/include/integrator_result_merger_container.hh"

#include "parachute_eom.h"

// These objects will automatically update the value of the variable you
// connect it to

// Each object has an add_integrable_object method

class ParachuteIntegrableObject : public er7_utils::IntegrableObject {
    public:

        ParachuteIntegrableObject(
            const bool&  init_complete,
            EOM_Outputs & output, 
            EOM_Work & work);

        virtual void create_integrators(
            const er7_utils::IntegratorConstructor &generator,
            er7_utils::IntegrationControls &controls,
            const er7_utils::TimeInterface &time_if);

        virtual void destroy_integrators();

        virtual void reset_integrators();

        virtual er7_utils::IntegratorResult integrate(
            double dyn_dt,
            unsigned int target_stage);
   
    protected: 

       virtual er7_utils::IntegratorResult trans_integ (
          double dyn_dt, unsigned int target_stage);

       virtual  er7_utils::IntegratorResult rot_integ (
          double dyn_dt, unsigned int target_stage);

       //References
       const bool& init_complete;
       EOM_Outputs& output;
       EOM_Work& work;

    private:

#ifndef SWIG

   /**
    * The object that merges integration results.
    */
   er7_utils::IntegratorResultMergerContainer
      integ_results_merger; //!< trick_units(--)

        jeod::RestartableT3SecondOrderODEIntegrator trans_integrator;
        jeod::RestartableSO3SecondOrderODEIntegrator rot_integrator;
#endif

        ParachuteIntegrableObject(const ParachuteIntegrableObject &);
        ParachuteIntegrableObject & operator=(const ParachuteIntegrableObject &);
};

#endif
