/*******************************TRICK HEADER************************************
PURPOSE:
  (Provide a generic interface to convert doubles into n-bit
   words using a user-specified:
   - resolution (i.e. significance of the least significant bit)
   - bit-size of each word (e.g. 8-bit and 32-bit)
   - number of words available to represent the double.

   This is the C++ -language version)

LIBRARY DEPENDENCY:
  ((../src/convert_double_to_uint_words.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2016) (ANTARES) (Initial version, uint only))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (IV&V code review clean up)))
*******************************************************************************/
#ifndef CML_CONVERT_DOUBLE_TO_WORDS_HH
#define CML_CONVERT_DOUBLE_TO_WORDS_HH

#include <cstdint>
#include <vector>
#include "cml/models/utilities/cml_message/include/cml_message.hh"

void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   std::vector<uint32_t> &words,
                                   unsigned int bit_size);

void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   uint32_t *words,
                                   const unsigned int array_size,
                                   unsigned int bit_size);

class ConvertDoubleToUintWords
{
protected:
  const double & convert_value; /* (--) User inputed value to be converted
                                        to a words value based off of specified
                                        bit size. */

  double resolution;       /* (--) Incrementation stepping for inputed
                                   value. */
  unsigned int word_count; /* (--) Number of words; size of the words vector. */
  unsigned int bit_size;   /* (--) Determines the max possible value a word
                                        can be. See max_uint. */

  std::vector<double> significance;/* (--) The amount by which the real
                                           number being represented by the
                                           words array will increment for every
                                           integral increment of the word at the
                                           same index. */

  double max_uint;                 /* (--) Max value each word can be as
                                           determined by bit size. */
  double max_uint_f;               /* (--) max_uint - 1 */
public:
  std::vector<uint32_t> words; /* (--) Output values, the set of integers
                                           that represent convert_value. */


  ConvertDoubleToUintWords( const double & convert_value,
                            double resolution,
                            unsigned int word_count,
                            unsigned int bit_size);
  virtual ~ConvertDoubleToUintWords() {};

  void update();
  void compute_significance();
  bool check_values();

private:
  ConvertDoubleToUintWords (const ConvertDoubleToUintWords& rhs);
  ConvertDoubleToUintWords & operator = (const ConvertDoubleToUintWords& rhs);
};

#endif