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

#ifndef CML_RANGE_FROM_PFIX_REFERENCE_HH
#define CML_RANGE_FROM_PFIX_REFERENCE_HH

#include "range_computation.hh"

namespace jeod {
class PlanetFixedPosition;
}

class RangeFromPfixReference : public RangeComputation
{
 public:
  explicit RangeFromPfixReference(const jeod::PlanetFixedPosition & state_in);
  ~RangeFromPfixReference() override = default;

  void set_reference_data() override;

 protected:
  void update_using_cartesian() override;
  void update_using_polar() override;
  void is_abstract() override {} // Makes this class instantiable

 private:
   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  RangeFromPfixReference(const RangeFromPfixReference& rhs);
  RangeFromPfixReference & operator = (const RangeFromPfixReference&);
};

#endif
