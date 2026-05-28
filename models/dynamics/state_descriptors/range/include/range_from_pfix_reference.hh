/********************************* TRICK HEADER *******************************
Purpose:
   (Specific implementation of the generic range computation for
   computation of range FROM a fixed reference location.)

Library Dependency:
   ((../src/range_from_pfix_reference.cc))

Programmers:
   (
    ((Gary Turner) (OSR)         (Sept 2014) (Initial version))
    ((Dan Jordan)  (Jacobs/JETS) (June 2017) (EM1 V&V cleanup))
   )

*******************************************************************************/

#ifndef CML_RANGE_FROM_PFIX_REFERENCE
#define CML_RANGE_FROM_PFIX_REFERENCE

#include "range_computation.hh"

namespace jeod {
class PlanetFixedPosition;
}

class RangeFromPfixReference : public RangeComputation
{
 public:
  explicit RangeFromPfixReference(const jeod::PlanetFixedPosition & state_in);
  virtual ~RangeFromPfixReference(){};

  virtual void set_reference_data() override;

 protected:
  virtual void update_using_cartesian() override;
  virtual void update_using_polar() override;
  virtual void is_abstract() override {}; // Makes this class instantiable

 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  RangeFromPfixReference(const RangeFromPfixReference& rhs);
  RangeFromPfixReference & operator = (const RangeFromPfixReference&);
};

#endif
