/*******************************TRICK HEADER************************************
PURPOSE:
  (Provide a generic interface to convert doubles into n-bit
   words using a user-specified:
   - resolution (i.e. significance of the least significant bit)
   - bit-size of each word (e.g. 8-bit and 32-bit)
   - number of words available to represent the double.

   This is the C-language version
   If possible, try to use the C++ version only)

LIBRARY DEPENDENCY:
  ((../src/double_to_uint_words.c))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2016) (ANTARES) (Initial version, uint only))
   ((Brent Caughron) (OSR) (June 2017) (Antares) (IV&V code review clean up)))
*******************************************************************************/

#ifndef CML_DOUBLE_TO_WORDS_H
#define CML_DOUBLE_TO_WORDS_H

// Function prototypes
#ifdef __cplusplus
extern "C" {
#endif

void double_to_uint_words( double convert_value,
                           double resolution,
                           unsigned int *words,
                           unsigned int  array_size,
                           unsigned int  bit_size);
#ifdef __cplusplus
}
#endif

#endif
