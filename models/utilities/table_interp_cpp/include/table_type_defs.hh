/*******************************TRICK HEADER***********************************
 PURPOSE:
    (Common typedefs used in Table Interpolation classes.)

 PROGRAMMERS:
   (((Robert Phillips) (OSR) (May 2018) (Antares) (initial version))
   )
*******************************************************************************/
#ifndef CML_TABLE_TYPE_DEFS_HH
#define CML_TABLE_TYPE_DEFS_HH

#include <cstddef>
#include<vector>

namespace cml {

  using SizeVec = std::vector<size_t>;
  using DoubleVec = std::vector<double>;
  using DoublePtrVec = std::vector<double*>;



} // namespace cml

#endif
