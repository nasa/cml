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

#ifndef ANTARES_GENERIC_SINGLE_INPUT_TABLE_HH
#define ANTARES_GENERIC_SINGLE_INPUT_TABLE_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "generic_multi_input_table.hh"

class GenericSingleInputTable : public GenericMultiInputTable
{
 friend class SingleInputTableVarDeriv;
 public:
  virtual bool initialize() override;
  // use the same constructors as the parent class.
  using GenericMultiInputTable::GenericMultiInputTable;
  GenericSingleInputTable() : GenericMultiInputTable(){};
 protected:
  virtual void generate_base_values() override;
  virtual bool generate_output() override;
 private:
  // Disable the copy/assignment operators
  GenericSingleInputTable (const GenericSingleInputTable&);
  GenericSingleInputTable& operator = (const GenericSingleInputTable&);
};
#endif
