/*******************************TRICK HEADER******************************
PURPOSE: (Wrapper Header for defining and applying vent disturbances to candidate vehicle)

LIBRARY DEPENDENCY:
  (../src/two_level_vent_set.cc)
  (../src/two_level_vent.cc)

PROGRAMMERS:
  (((Suyog Benegalrao & Alexandre Masset) (JSC) (Jul 2025) (Antares) (initial version)))
 ************************************************************************/

#ifndef CML_TWO_LEVEL_VENT_SET_HH
#define CML_TWO_LEVEL_VENT_SET_HH

#include "cml/models/fhw/effectors/vent/include/vent_set.hh"

#include <random> /* need for std::mt19937 */

class TwoLevelVent;

class TwoLevelVentSet : public VentSet
{
  friend class TwoLevelVent;

    public:
	/* Randomization Variables */
    int rand_seed; /* (--)
        random seed used for vent duration and wait time randomization */

    protected:
    /* External Reference Mass */
    const double& vehicle_mass;    /* (kg) 
        mass of candidate vehicle */
    std::mt19937 gen; /* (--)
        random number generator for vent duration and wait time randomization */

    public:
    /* Constructor */
    TwoLevelVentSet( jeod::DynBody& dyn_body_,
                     const double& time);
    TwoLevelVentSet(size_t num_vents,
          jeod::DynBody & dyn_body_,
          const double & time);

    /* Methods */
    void initialize() override;
    TwoLevelVent * get_two_level_vent(size_t ii);

    protected:
    void deactivate() override;

    private:

    /* Copy constructor, operator= declared private and not implemented; class is not copyable. */
    TwoLevelVentSet(const TwoLevelVentSet&);
    TwoLevelVentSet& operator = (const TwoLevelVentSet&);
};
#endif
