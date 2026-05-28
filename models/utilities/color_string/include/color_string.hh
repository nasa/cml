/*******************************************************************************
Purpose: (Apply ANSI color codes to strings in order to change the color of the
          text or the background of the text to a desired color.)
*******************************************************************************/

#ifndef COLOR_STRING_HH
#define COLOR_STRING_HH

#include <string>

class ColorString {
  public:
    // This set of static casts will turn the inputed text into the
    // desired colors.
    static std::string black(const std::string &input) {
      return "\033[30m" + input + "\033[0m";
    }

    static std::string red(const std::string &input) {
      return "\033[31m" + input + "\033[0m";
    }

    static std::string green(const std::string &input) {
      return "\033[32m" + input + "\033[0m";
    }

    static std::string yellow(const std::string &input) {
      return "\033[33m" + input + "\033[0m";
    }

    static std::string blue(const std::string &input) {
      return "\033[34m" + input + "\033[0m";
    }

    static std::string magenta(const std::string &input) {
      return "\033[35m" + input + "\033[0m";
    }

    static std::string cyan(const std::string &input) {
      return "\033[36m" + input + "\033[0m";
    }

    // While the ascii color code here is defined as white, the color
    // that appears looks more grey than white.
    static std::string white(const std::string &input) {
      return "\033[37m" + input + "\033[0m";
    }

    static std::string bright_yellow(const std::string &input) {
      return "\033[93m" + input + "\033[0m";
    }

    // This next set of static casts will turn the background behind
    // the inputed text into the desired colors.
    static std::string black_background(const std::string &input) {
      return "\033[40m" + input + "\033[0m";
    }

    static std::string red_background(const std::string &input) {
      return "\033[41m" + input + "\033[0m";
    }

    static std::string green_background(const std::string &input) {
      return "\033[42m" + input + "\033[0m";
    }

    static std::string yellow_background(const std::string &input) {
      return "\033[43m" + input + "\033[0m";
    }

    static std::string blue_background(const std::string &input) {
      return "\033[44m" + input + "\033[0m";
    }

    static std::string magenta_background(const std::string &input) {
      return "\033[45m" + input + "\033[0m";
    }

    static std::string cyan_background(const std::string &input) {
      return "\033[46m" + input + "\033[0m";
    }

    // While the ascii color code here is defined as white, the color
    // that appears looks more grey than white.
    static std::string white_background(const std::string &input) {
      return "\033[47m" + input + "\033[0m";
    }
};

#endif
