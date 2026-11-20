/*******************************TRICK HEADER******************************
PURPOSE: (Wrapper Source for defining and applying vent disturbances to candidate vehicle)

PROGRAMMERS:
  (((Suyog Benegalrao & Alexandre Masset) (JSC) (Jul 2025) (Antares) (initial version)))
 ************************************************************************/

// Headers
#include "../include/two_level_vent_set.hh"
#include "../include/two_level_vent.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

/***************************************************************************************************
Constructors
***************************************************************************************************/

TwoLevelVentSet::TwoLevelVentSet( jeod::DynBody& dyn_body_,
                        const double& time)
    :
    VentSet::VentSet(dyn_body_, time),
    rand_seed(0),
    vehicle_mass(dyn_body_.mass.composite_properties.mass),
    gen( std::mt19937(static_cast<std::mt19937::result_type>(rand_seed)))
{
    /* start active vents at activation */
    start_vents_at_activation = true;
    subscribe_name = "TwoLevelVent:";
}


TwoLevelVentSet::TwoLevelVentSet(
    size_t            num_vents,
    jeod::DynBody         & dyn_body_,
    const double    & time)
  :
  VentSet::VentSet(dyn_body_, time),
  rand_seed(0),
  vehicle_mass(dyn_body.mass.composite_properties.mass),
  gen( std::mt19937(static_cast<std::mt19937::result_type>(rand_seed)))
{

    /* start active vents at activation */
    start_vents_at_activation = true;
    subscribe_name = "TwoLevelVent:";

    for (size_t ii = 0; ii < num_vents; ++ii) {
        TwoLevelVent * new_vent = new TwoLevelVent(time, *this);
        /* assign names */
        new_vent->name = "Vent" + std::to_string(ii+1);
        new_vent->allocated_in_set = true;

    }
}

/***************************************************************************************************
TwoLevelVentSet
deactivate: Override method when model is unsubscribed. Reset to low mode every two_level vent to
guarantee they start in high mode if activated at a later time
***************************************************************************************************/
void TwoLevelVentSet::deactivate()
{
    // Resets mode and venting flag
    stop_vents();
    VentSet::deactivate();
}

/***************************************************************************************************
TwoLevelVentSet
get_two_level_vent: Gets TwoLevelVent pointer. Fails if not index is out of range.
***************************************************************************************************/
TwoLevelVent* TwoLevelVentSet::get_two_level_vent(size_t ii) {
    // Get vent from VentSet method. It has its own checks.
    SimpleVent* vent = VentSet::get_vent(ii);
    // Recast into a TwoLevelVent. If it fails, the pointer is null.
    TwoLevelVent* two_level_vent = dynamic_cast<TwoLevelVent*>(vent);
    if (two_level_vent == nullptr) {
        CMLMessage::fail(
        __FILE__,__LINE__,"Invalid request\n"
        "Index ", ii, " is not a TwoLevelVent instance.\n");
    }
    return two_level_vent;
}

/***************************************************************************************************
initialize: Python Call Initializations
***************************************************************************************************/
void TwoLevelVentSet::initialize(){
    // Safety net.
    if ( not MathUtils::has_changed_from(vehicle_mass, 0.0) ) {
        CMLMessage::error(
                __FILE__,__LINE__,"Illegal setup\n"
            "Vehicle mass cannot be zero.\n"
            "two_level_venting instruction not applied.\n");
    }
    /* Calls SimpleVent::initialize()
    Ensures the vents can be operated through CML Vent checks. Updates initialized boolean. */
    VentSet::initialize();
}
