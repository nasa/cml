/*******************************TRICK HEADER************************************
PURPOSE:
  (Provide a generic C++ interface to convert doubles into n-bit
   words using a user-specified:
   - resolution (i.e. significance of the least significant bit)
   - bit-size of each word (e.g. 8-bit, 32-bit)
   - number of words available to represent the double.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2016) (ANTARES) (Initial version, uint only))
   ((Brent Caughron) (OSR) (June 2017) (ANTARES)
          (Cleanup/rewriting following code review))
   ((Bingquan Wang) (OSR) (Jan 2018) (ANTARES) (migrated to gcc 4.8))
   ((Tony Varesic) (OSR) (May 2021) (ANTARES) (added bit_size check)))
*******************************************************************************/
//Local Headers
#include "../include/convert_double_to_words.hh"
#include <cstddef> //for NULL

/*******************************************************************************
Purpose:(Run the class function that calls the variables and creates the
         words array that is populated with the converted values for
         the original variable pass in format.)
*******************************************************************************/
void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   uint32_t *words,
                                   const unsigned int array_size,
                                   unsigned int bit_size)
{
  if (words == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid arguments.\n",
      "The words array was passed in as NULL. There is nowhere for the\n"
      "converted values to go.\n");
      return;
  }

  ConvertDoubleToUintWords local_conv( convert_value,
                                       resolution,
                                       array_size,
                                       bit_size);
  local_conv.update();

  // Copy the class data to the output.
  for ( unsigned int ii = 0; ii < array_size; ii++) {
    words[ii] = local_conv.words.at(ii);
  }
}

/*******************************************************************************
Purpose:(Run the class function that calls the variables and creates the
         words array that is populated with the converted values.)
*******************************************************************************/
void convert_double_to_uint_words( double convert_value,
                                   double resolution,
                                   std::vector<uint32_t> &words,
                                   unsigned int bit_size)
{
  ConvertDoubleToUintWords local_conv( convert_value,
                                       resolution,
                                       words.size(),
                                       bit_size);

  local_conv.update();

  // Copy the class data to the output.
  words = local_conv.words;
}

/*******************************************************************************
ConvertDoubleToUintWords
Purpose:(Defines the class constructor for the object local_conv.)
*******************************************************************************/
ConvertDoubleToUintWords::ConvertDoubleToUintWords( const double & in_convert_value,
                                                    double in_resolution,
                                                    unsigned int in_word_count,
                                                    unsigned int in_bit_size)
  :
  convert_value(in_convert_value),
  resolution(in_resolution),
  word_count(in_word_count),
  bit_size(in_bit_size)
{
  compute_significance();
  words.assign(word_count,0);
  if (word_count == 0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n\n",
      "Specified that the double-to-words model will generate 0 words.\n"
      "This is probably not useful and probably an error.\n"
      "Check configuration at the point from which this constructor was "
      "called.\n");
  }
  if (bit_size > 32) { // the words array can only hold up to 32-bit integers.
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n\n",
      "The requested bit_size cannot be > 32.\n"
      "The generated words are assigned to uint32_t types.\n"
      "Setting bit-size to 32 to continue.\n");
    bit_size = 32;
  }
}

/*******************************************************************************
Purpose:(Check passed in values to see if they're in acceptable tolerance
         or to see if they're initialized correctly.)
*******************************************************************************/
bool
ConvertDoubleToUintWords::check_values()
{
  // Check for too small
  // The 0.5 is there because if convert_value is >= 0.5*resolution but still
  // below resolution it can still round up to be equal to the resolution and
  // still give the correct words conversion value.
  if (convert_value < 0.5*resolution) {
    words.assign(word_count,0);
    return false;
  }

  // If convert_value is too large - i.e. is greater than or equal to the least
  // significant bit of the first word beyond the array -- which is at index
  // word_count -- set all words to their max value and leave.
  if (convert_value >= significance.at(word_count) - 0.5*resolution) {
    for (unsigned int ii = 0; ii < word_count; ii++) {
      words.at(ii) = max_uint_f;
    }
    // If the if statement is tripped write out a message with the inform
    // severity so that it can be viewed as wanted by the user
    CMLMessage::inform(
      __FILE__,__LINE__,"Roundoff warning:\n\n",
      "inputvalue ", convert_value, " is too large to be represented by words.\n"
      "The output will be the max possible value (", significance.at(word_count)-resolution, ") instead.\n");
    return false;
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
    //error message
    CMLMessage::error(
      __FILE__,__LINE__,"Initialization error:\n\n",
      "Resolution of words is lower than the resolution of double.\n "
      "Not all words are available for value ", convert_value, ".\n");
  }
  return true;
}

/*******************************************************************************
compute_significance
Purpose:(Class function that calculates the significance at each word-array
         column and is then used in calculations to convert the inputed value
         into words.)
*******************************************************************************/
void
ConvertDoubleToUintWords::compute_significance()
{

  // First identify the largest value expressable by an unsigned int
  max_uint = 2.0*(1UL<<(bit_size-1));
  max_uint_f = max_uint - 1.0;

  // Find the first (i.e. least-significant) zero element of the word-array
  double sig_ = resolution;
  for (unsigned int ii = 0; ii < word_count; ++ii) {
    significance.push_back( sig_);
    sig_ *= max_uint;
  }
  significance.push_back( sig_); // The smallest value that is larger than
                                 // anything that can be represented by
                                 // maximizing every element in the words
                                 // vector.

  unsigned int max_index = 0;
  while (convert_value >= sig_ && max_index < word_count) {
    max_index++;
    sig_ *= max_uint; // significance of least-significant-bit
                              // of word at new max_index
  }
}

/*******************************************************************************
Purpose:(Class function that updates the words array with converted
         variable values.)
*******************************************************************************/
void
ConvertDoubleToUintWords::update()
{

  if (!check_values()) {
    return;
  }
  // Otherwise, work backward from the most significant uint to the least
  // Note that the value of the significance of the least-signifiant bit of the
  // word that (is / would be) found at index max_index is larger than
  // convert_value, so the max_index word must be zero, and the most
  // significant non-zero word is at max_index -1.
  double remainder = convert_value;
  for (int word_id = word_count -1; word_id >= 0; word_id--) {
    // Take the whole multiple of the significance of this word
    words.at(word_id) = static_cast<unsigned int> (remainder /
                                                   significance.at(word_id));

    // Keep the remainder.
    remainder -= words.at(word_id) * significance.at(word_id);
  }

  // Check to see if round off error occurs causing lost value
  // If round off error does not occur leave
  if (remainder < 0.5*resolution) {
    return;
  }

  // else
  // Check for how round off error will affect words values
  // Run through words values until the first non (max_uint_f) values is
  // found and then up it by one and make all (max_uint_f) values to the
  // left of it zero to account for the upping by 1
  for (unsigned int ii = 0; ii < word_count; ii++) {
    if (words[ii] < max_uint_f) {
      for (unsigned int jj = 0; jj < ii; jj++) {
        words[jj] = 0;
      }
      words[ii] += 1;
      return;
    }
  }
}