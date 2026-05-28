/********************************* TRICK HEADER *******************************
Purpose:
   (Specific implementtation of the generic range computation for
   computation of range TO a fixed reference location.)

Library Dependency:
   ((../src/range_to_pfix_reference.cc))

Programmers:
   (
    ((Gary Turner) (OSR)         (Sept 2014) (Initial version))
    ((Dan Jordan)  (Jacobs/JETS) (June 2017) (EM1 V&V cleanup))
   )

*******************************************************************************/

#ifndef CML_RANGE_TO_PFIX_REFERENCE
#define CML_RANGE_TO_PFIX_REFERENCE

#include "range_computation.hh"

class PlanetFixedPosition;

class RangeToPfixReference : public RangeComputation
{
 protected:
  const double (&direction)[3]; /* (--)
    Pointer to a vector used to update the reference
    direction. This is typically the vehicle's velocity. */
  double dummy_direction[3]; /* (--)
    Arbitrary spec in case the direction is not externally specified.*/

 public:
  explicit RangeToPfixReference( const jeod::PlanetFixedPosition & state_in);
  RangeToPfixReference( const jeod::PlanetFixedPosition & state_in,
                        const double (&direction)[3]);
  virtual ~RangeToPfixReference(){};

  virtual void set_reference_data() override;
  void update ();

 protected:
  virtual void update_using_cartesian() override;
  virtual void update_using_polar() override;
  virtual void is_abstract() override {}; // Makes this class instantiable

 private:

   // Copy constructor and assignment operator for this class are
   // declared private and are not implemented.
  RangeToPfixReference(const RangeToPfixReference& rhs);
  RangeToPfixReference & operator = (const RangeToPfixReference&);

};

#endif
