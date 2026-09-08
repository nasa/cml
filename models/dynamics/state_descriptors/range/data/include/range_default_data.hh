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

// TODO Nino Tarantino: this needs to be a single free function, not a class.
class RangeDefaultData
{
  public:
  RangeDefaultData() = default;
  virtual ~RangeDefaultData() = default;
  RangeDefaultData (const RangeDefaultData& rhs) = delete;
  RangeDefaultData& operator =  (const RangeDefaultData& rhs) = delete;

  void launch_pad_KSC_39B(RangeFromPfixReference & pad) const;
};
#endif
