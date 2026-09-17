/*******************************TRICK HEADER************************************
PURPOSE:
  (Provide a generic interface to convert doubles into n-bit
   words using a user-specified:
   - resolution (i.e. significance of the least significant bit)
   - bit-size of each word (e.g. 8-bit and 32-bit)
   - number of words available to represent the double.)

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

/**
 * Convert a `double` into n-bit unsigned integer words
 *
 * @param[in]     convert_value Value to convert
 * @param[in]     resolution    Significance of the least significant bit
 * @param[in,out] words         Output vector to store the converted data
 * @param[in]     bit_size      Number of bits in each word
 *
 * @pre The size of `words` must be sized to the number of words that should be
 *      used to store the converted data.
 *
 * For example, convert a double-precision value to a set of four unsigned integer
 * words, each of size 8 bits, with a resolution of 0.1 per bit:
 * @code
 * std::vector<unit32_t> words(4);
 * convert_double_to_uint_words(value, 0.1, words, 8U);
 * @endcode
 *
 * @note `words` is returned with the _least-significant-word_ at position 0.
 */
void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   std::vector<uint32_t> &words,
                                   unsigned int bit_size);

/**
 * Convert a `double` into n-bit unsigned integer words
 *
 * @param[in]  convert_value Value to convert
 * @param[in]  resolution    Significance of the least significant bit
 * @param[out] words         Output array to store the converted data
 * @param[in]  array_size    Size of the output array, i.e. number of words to use
 * @param[in]  bit_size      Number of bits in each word
 *
 * For example, convert a double-precision value to a set of four unsigned integer
 * words, each of size 8 bits, with a resolution of 0.1 per bit:
 * @code
 * unit32_t words[4];
 * convert_double_to_uint_words(value, 0.1, words, std::size(words), 8U);
 * @endcode
 *
 * @note `words` is returned with the _least-significant-word_ at position 0.
 */
void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   uint32_t *words,
                                   const unsigned int array_size,
                                   unsigned int bit_size);

/**
 * Provides an interface to convert a `double` into n-bit unsigned integer words
 */
class ConvertDoubleToUintWords
{
protected:
  const double & convert_value; /**< (--) User inputted value to be converted
                                          to a words value based off of specified
                                          bit size. */

  double resolution;       /**< (--) Incrementation stepping for inputted value. */
  unsigned int word_count; /**< (--) Number of words; size of the words vector. */
  unsigned int bit_size;   /**< (--) Determines the max possible value a word
                                     can be. @see max_uint. */

  std::vector<double> significance; /**< (--) The amount by which the real
                                              number being represented by the
                                              words array will increment for every
                                              integral increment of the word at the
                                              same index. */

  double max_uint{0.0}; /**< (--) Max value each word can be as determined by bit size. */
  double max_uint_f{0.0}; /**< (--) #max_uint - 1 */
public:
  std::vector<uint32_t> words; /**< (--) Output values, the set of integers
                                         that represent #convert_value.
                                         @note The least-significant word is at index 0. */

  /**
   * Constructor
   *
   * @param[out] in_convert_value Reference to value to convert
   * @param[in]  in_resolution    Significance of the least significant bit
   * @param[in]  in_word_count    Number of words to use for the conversion
   * @param[in]  in_bit_size      Number of bits in each word
   *
   * For example, set up the class to convert a double-precision value to a set
   * of four unsigned integer words, each of size 8 bits, with a resolution of
   * 0.1 per bit:
   * @code
   * ConvertDoubleToUintWords converter(value_ref, 0.1, 4U, 8U);
   * @endcode
   */
  ConvertDoubleToUintWords( const double & in_convert_value,
                            double in_resolution,
                            unsigned int in_word_count,
                            unsigned int in_bit_size);

  /**
   * Copy constructor deleted.
   */
  ConvertDoubleToUintWords (const ConvertDoubleToUintWords&) = delete;

  /**
   * Copy assignment operator deleted.
   */
  ConvertDoubleToUintWords & operator = (const ConvertDoubleToUintWords&) = delete;

  /**
   * Perform the conversion.
   */
  void update();

  /**
   * Calculates the significance of each word-array column
   */
  void compute_significance();

  /**
   * Checks to make sure that the value to convert is convertible
   *
   * @return `true` if conversion is possible, otherwise `false`
   */
  bool check_values();
};

#endif
