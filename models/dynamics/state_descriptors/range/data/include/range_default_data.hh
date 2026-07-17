/*************************************************************
PURPOSE:
   (Provide default data for launch and landing sites.

LIBRARY DEPENDENCY:
   ((../src/range_default_data.cc))

PROGRAMMERS:

    ((Gary Turner) (Odyssey) (January 2015) (Antares) (Initial version)))

*************************************************************/

#ifndef CML_RANGE_DEFAULT_DATA_HH
#define CML_RANGE_DEFAULT_DATA_HH

#include "../../include/range_from_pfix_reference.hh"

class RangeDefaultData
{
  public:
  RangeDefaultData(){};
  virtual ~RangeDefaultData(){};

  void launch_pad_KSC_39B(RangeFromPfixReference & launch);

private:
   // Not implemented:
   RangeDefaultData (const RangeDefaultData& rhs);
   void operator =  (const RangeDefaultData& rhs);;
};
#endif
