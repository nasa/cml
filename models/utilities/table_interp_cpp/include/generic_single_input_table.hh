/*******************************TRICK HEADER***********************************
 PURPOSE:
    ( Provide a simplified form of GenericMultiInputTable for the simple
      case of using only one independent variable.
      This simplifies the form of the method generate_base_values() and
      generate_output.)

 LIBRARY DEPENDENCY:
    ((../src/generic_single_input_table.cc)
    )

 PROGRAMMERS:
   (((Gary Turner) (OSR) (May 2023) (Antares) (initial version))
   )
*******************************************************************************/

#ifndef CML_GENERIC_SINGLE_INPUT_TABLE_HH
#define CML_GENERIC_SINGLE_INPUT_TABLE_HH


#include "generic_multi_input_table.hh"

class GenericSingleInputTable : public GenericMultiInputTable
{
 friend class SingleInputTableVarDeriv;
 public:
  bool initialize() override;
  // use the same constructors as the parent class.
  using GenericMultiInputTable::GenericMultiInputTable;
  GenericSingleInputTable (const GenericSingleInputTable&) = delete;
  GenericSingleInputTable& operator = (const GenericSingleInputTable&) = delete;
 protected:
  void generate_base_values() override;
  bool generate_output() override;
};
#endif
