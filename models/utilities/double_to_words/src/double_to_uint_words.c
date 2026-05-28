/*******************************TRICK HEADER************************************
PURPOSE:
  (Provide a generic interface (in C) to convert doubles into n-bit
   words using a user-specified:
   - resolution (i.e. significance of the least significant bit)
   - bit-size of each word (e.g. 8-bit, 32-bit)
   - number of words available to represent the double.

   If possible, try to use the C++ version only)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2016) (ANTARES) (Initial version, uint only))
   ((Brent Caughron) (OSR) (June 2017) (ANTARES)
          (Cleanup/rewriting following code review))
   ((Tony Varesic) (OSR) (May 2021) (ANTARES) (added bit_size check)))
*******************************************************************************/
#include "../include/double_to_words.h"
#include <stdio.h>

// NOTE - written in C for maximum utility.

void double_to_uint_words( double convert_value,
                           double resolution,
                           unsigned int *words,
                           unsigned int  array_size,
                           unsigned int  bit_size)
{

  if (words == NULL) {
    fprintf(stderr, "Invalid argument for words \n");
    fprintf(stderr, "Words variable needs to be initialized\n");
    return;
  }
  if (bit_size > 32) { // the words array can only hold up to 32-bit integers.
    fprintf(stderr, "Invalid argument for bit_size \n");
    fprintf(stderr, "The requested bit_size cannot be > 32\n");
    return;
  }

  // initialize the values
  unsigned int ii= 0;
  for (ii = 0; ii < array_size; ii++) {
    words[ii] = 0;
  }

  // Check for too small
  if (convert_value < resolution) {
    fprintf(stderr, "'convert_value' less than 'resolution'. Returning out.\n");
    return;
  }

  // Check whether the specified separation between words ("resolution") is
  // finer than the separation between adjacen double values.  If it is, there
  // will be words that are not available because they will be skipped before
  // reaching the next value available using the double data type.
  //
  // double is 64-bit:
  // The IEEE 754 standard specifies a binary64 as having:
  //   Sign bit: 1 bit
  //   Exponent: 11 bits
  //   Significand precision: 53 bits (52 explicitly stored)
  // 2^53 = 9,007,199,254,740,992, the largest integer preresentable
  // in 53 bits of binary64
  //
  // Thus the next nearest value to convert_value that can be expressed as a
  // double will be approximately 1 part in 9,007,199,254,740,992 away from
  // the current value.  If that distance is larger than "resolution", then
  // there will be words that fit in the gap that will therefore be unusable.
  // Rather than using division (and the risk of sub-normal numbers) and writing
  //     "if (resolution <= convert_value / 9,007,199,254,740,992)",
  // the test is structured with multiplication:
  if (convert_value >= (0.900719926*1e16)*resolution) {
    fprintf(stderr,"double_to_uint_words.c:%d \n\n",__LINE__ - 1);
    fprintf(stderr,"Resolution of words is lower than the resolution of double.\n");
    fprintf(stderr,"Not all words are available for value %e.\n", convert_value);
  }

  // First identify the largest value expressable by an unsigned int:
  // max_uint is the number of values expressable by a variable of size
  // bit_size; an unsigned int includes 0 so the largest value expressable is
  // 1 less than max_uint, max_uint_f.
  unsigned long max_uint   = 2*(1UL<<(bit_size-1));
  unsigned long max_uint_f = max_uint - 1;

  // Find the first (i.e. least-significant) zero element of the word-array
  double significance = resolution;
  unsigned int max_index = 0;
  while (convert_value >= significance && max_index < array_size) {
    max_index++;
    significance *= max_uint; // significance of least-significant-bit
                              // of word at new max_index
  }

  // If convert_value is too large - i.e. is greater than or equal to the least
  // significant bit of the first word beyond the array, set all words to
  // their max value and leave.
  if (convert_value >= significance-0.5*resolution) { // (and max_index == array_size)
    for (ii = 0; ii < array_size; ii++) {
      words[ii] = (unsigned int) max_uint_f;
    }
    fprintf(stderr,"double_to_uint_words.c:%d \n\n",__LINE__ - 1);
    fprintf(stderr,"inputvalue %e is too large to be represented by words.\n",
            convert_value);
    fprintf(stderr,"The output will be the max possible value (%e) instead.\n",
            significance-resolution);
    return;
  }

  // Otherwise, work backward from the most significant uint to the least
  // Note that the value of the significance of the least-signifiant bit of the
  // word that (is / would be) found at index max_index is larger than
  // convert_value, so the max_index word must be zero, and the most
  // significant non-zero word is at max_index -1.
  double remainder = convert_value;
  int word_id;
  for (word_id = max_index -1; word_id >= 0; word_id--) {
    // regenerate the significance for each word; stepping back down using a
    // divide would be faster, but can lead to numerical error.
    significance = resolution;
    // Find the significance of the word beyond the array
    int jj = 0;
    for (jj = 0; jj < word_id; jj++) {
      significance *= max_uint;
    }

    // Take the whole multiple of the significance of this word
    // words[word_id] = static_cast<unsigned int> (remainder / significance);
    words[word_id] = (unsigned int) (remainder / significance);

    // Keep the remainder.
    remainder -= words[word_id] * significance;
  }

  // Check to see if round off error occurs causing lost value
  // If round off error doesnt occur, leave
  if (remainder < 0.5*resolution) {
    return;
  }

  // else
  // Check for how round off error will affect words values
  // Run through words values until the first non (max_uint_f) values is
  // found and then up it by one and make all (max_uint_f) values to the
  // left of it zero to account for the upping by 1
  for (ii = 0; ii < array_size; ii++) {
    if (words[ii] < max_uint_f) {
      for (unsigned int jj = 0; jj < ii; jj++) {
        words[jj] = 0;
      }
      words[ii] += 1;
      return;
    }
  }
}
