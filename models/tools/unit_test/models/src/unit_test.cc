/*********************************** TRICK HEADER **************************
PURPOSE:    (To provide a Trick-friendly unit-test framework)
REFERENCES: (../models-C by Jason Arnold)

PROGRAMMERS:
  (((Jason Arnold) (Titan) (Jul 2005))
   ((Gary Turner) (OSR) (Oct 2016)
    (transition from char*, malloc, free, sprintf, etc. to use STL strings,
    vectors, and lists))
  )
***************************************************************************/
#include <cmath>      // fabs
#include <cfloat>     // DBL_EPSILON
#include <algorithm>  // std::max
#include <iostream>   // std::cout
#include <sstream>    // std::istringstream
#include <fstream>    // std::ifstream
#include <pthread.h>  // for pthreads - sending commands.
#include <regex>

#include "../include/unit_test.hh"

#include "cml/models/utilities/math_utils/include/math_utils.hh"

#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include "trick/input_processor_proto.h"
#include "trick/IPPython.hh"

extern Trick::IPPython* the_pip;


/*****************************************************************************
Constructor
*****************************************************************************/
SweepSet::SweepSet(
            double & variable_in,
            double start_in,
            double end_in,
            double incr_in)
  :
  variable(variable_in),
  start( start_in),
  end( end_in),
  increment( incr_in),
  value(start),
  epsilon(0.0)
{
  // epsilon is the threshold around "end" that determines 'close-enough'
  // If adjusting this, the magic number (1.0E-12) should be larger than
  // DBL_EPSILON
  epsilon = std::fabs(end - start) * 1.0E-12;

  // Make sure that incrementing "value" will result in a change to
  // "value" through the entire range start -> end.
  // increment must be larger than DBL_MIN and larger than DBL_EPSILON *
  // every value in the range.
  // Checking the end points only is a sufficient test; at least one of the
  // end points must represent the maximum (absolute) value of the range, so if
  // the increment is not so small that it will fail to cause a change to either
  // of the (large) endpoints, it will be sufficiently large to cause a change
  // throughout the domain.
  // If start and end are identical, increment can be any value, including 0.0.
  if ((!MathUtils::is_near_equal(start, end)) &&  // i.e. start != end
      ((std::abs(increment) < std::abs(start) * DBL_EPSILON) ||
       (std::abs(increment) < std::abs(end) * DBL_EPSILON))) {
    increment = std::max( std::max(std::abs(start), std::abs(end)) * DBL_EPSILON,
                          DBL_MIN);
    if (end < start) {
      increment *= -1.0;
    }
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inputs\n",
      "The specified value of increment is too small for numerical precision."
      "\nResetting the value to ", increment, ".\n");
  }

  // Check the sign of increment to make sure it is going the correct way:
  if ((end - start) * increment < 0.0) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid inputs\n",
      "Sweep increment is going the wrong direction.\n"
      "Switching sign; results may be unexpected.\n");
    increment *= -1.0;
  }

  // Warn when sweeping through > 1000 data point just because that is
  // probably a user-error but not an inherent risk to code stability.
  // NOTE - div-0 is protected here; the only way that increment=0 is if
  // end=start (due to previous checks); in that case, the if test will return
  // false and the "/increment" will not be executed.
  if (std::abs(increment) * 1000.0 < std::abs(end - start)) {
    CMLMessage::warn(
      __FILE__,__LINE__,"Excessive data\n",
      "Sweep will require %u data points.\n"
      "This could take a while.  Verify intent.\nContinuing.\n",
      static_cast<int>(std::abs((end - start)/increment)));
  }
}
/*****************************************************************************
Copy Constructor
*****************************************************************************/
SweepSet::SweepSet(const SweepSet &other) :
  variable(other.variable),
  start(other.start),
  end(other.end),
  increment(other.increment),
  value(other.value),
  epsilon(other.epsilon)
{}
/*****************************************************************************
Assignment Operator
*****************************************************************************/
SweepSet& SweepSet::operator = (const SweepSet &other)
{
  if (this == &other) return *this;
  variable = other.variable;
  start = other.start;
  end = other.end;
  increment = other.increment;
  value = other.value;
  epsilon = other.epsilon;
  return *this;
}
/****************************************************************************/
UnitTestFramework::UnitTestFramework()
  :
  enabled(true),
  cycle_data(false),
  initialized(false),
  using_sweeps(false),
  first_sweep(true),
  sweeps_complete(false),
  warning_on_sim_end(true),
  cycle_overruns(0),
  cycle_overruns_limit(2)
{}

/*****************************************************************************
increment_sweep
Purpose: (Increments the sweep variable.  Returns true if it resulted in the
          sweep completing in which case the variable gets reset to its start
          value.)
*****************************************************************************/
bool SweepSet::increment_sweep()
{
  // initialize with sweep not having finished
  bool reset_sweep = false;
  // to_go is the remaining "distance" to the end-point, in the direction of
  // the increment.  Note - not the absolute value of the "distance" to go,
  // this must be allowed to go negative if it overshoots, mainly for the
  // first pass after which point that should be protected.
  double to_go = (increment > 0) ? (end - value) : (value - end);

  // If within the allowable tolerance of the end-point, tag the sweep as
  // completing and reset the value.
  if ( to_go - epsilon <= 0.0) {
    value = start;
    reset_sweep = true;
  }
  // otherwise, increment the value
  else {
    value += increment;
    // if overshot, set value to the end point of the sweep
    // (overshot if (value-end) and increment have the same sign).
    if ( (value - end) * increment > 0) {
      value = end;
    }
  }
  // push the new value onto the target variable.
  variable = value;
  return reset_sweep;
}


/*****************************************************************************
add_sweep
Purpose:(Adds a sweep variable to the unit tests)
*****************************************************************************/
void UnitTestFramework::add_sweep(
        double & variable,
        double start,
        double end,
        double incr)
{
  if (initialized) {
    CMLMessage::error(
    __FILE__,__LINE__," Invalid input sequence.\n",
    "Sweeps must be added before initialization.\n");
  }
  sweeps.push_back(SweepSet(variable, start, end, incr));
  using_sweeps = true;
}

/*****************************************************************************
initialize
Purpose:(Parses the variables file and the data file to provide a list of
         variables to assign and a list of commands that assigns data to those
         variables)
*****************************************************************************/
void
UnitTestFramework::initialize()
{
  if (!enabled) {
    return;
  }

  if (using_sweeps) {
    configure_sweeps();
  }
  else if ( !linked_vars_file_name.empty()) {
    configure_file_combinations();
  }
  else {
    configure_from_definition_file();
  }
  initialized = true;
  return;
}

/*****************************************************************************
update
Purpose:(Each time this gets executed, a new line of input data gets applied.)
*****************************************************************************/
void
UnitTestFramework::update()
{
  if (!enabled || !initialized) {
    return;
  }

  if (using_sweeps) {
    if (first_sweep) { // use initialization values without updates.
      first_sweep = false;
      return;
    }
    update_sweeps();
  }
  else {
    update_file();
  }
  return;
}

/*****************************************************************************
configure_sweeps
Purpose:(Configures the framework for using sweeps and calls the
         initialize method for each sweep-set)
*****************************************************************************/
void
UnitTestFramework::configure_sweeps()
{
  //sweeps and data-files are incompatible.  If both are defined, fail
  if (!vars_file_name.empty() || !data_file_name.empty() ||
      !linked_vars_file_name.empty()) {
    CMLMessage::fail(
      __FILE__,__LINE__,"invalid configuration\n",
      "Specified a sweep-based input and filenames.\n"
      "Incompatible methods.  Cannot distinguish which to use."
      "If both are needed, use 2 framework instances.\n");
  }
  for( std::list<SweepSet>::iterator it = sweeps.begin();
       it != sweeps.end();
       ++it) {
    it->initialize();
  }
}

/*****************************************************************************
configure_file_combinations
Purpose:(Used when the input data is specified in multiple files; all inputs
         from all input data files will be combined, with every statistical
         combination being tested.)
*****************************************************************************/
void
UnitTestFramework::configure_file_combinations()
{
  if (!vars_file_name.empty() || !data_file_name.empty()) {
    CMLMessage::fail(
      __FILE__,__LINE__,"invalid configuration\n",
      "Specified a linked variable file name and independent variable or\n"
      "data file names.\n"
      "Incompatible methods.  Cannot distinguish which to use."
      "If both are needed, use 2 framework instances.\n");
  }

  populate_linked_variables();
  process_linked_variables();
}

/*****************************************************************************
expand_env_variables
Purpose: (Replaces environment variable placeholders in the form `${VAR_NAME}`
          within the input string with their corresponding values from the
          process environment. The search pattern matches variable names
          beginning with a letter or underscore, followed by letters, digits,
          or underscores. If an environment variable is found, its value is
          inserted into the output string. If a variable is not set, the
          placeholder is left unchanged, a warning is printed via `CMLMessage::error`,
          and a runtime exception is thrown. The method preserves any text
          outside of `${}` sequences unchanged.)
*****************************************************************************/
std::string UnitTestFramework::expand_env_variables(const std::string& input) {
    static const std::regex pattern(R"(\$\{([A-Za-z_][A-Za-z0-9_]*)\})");

    std::string result;
    std::sregex_iterator begin(input.begin(), input.end(), pattern);
    std::sregex_iterator end;

    std::size_t last_pos = 0;

    for (auto it = begin; it != end; ++it) {
        const std::smatch& match = *it;
        result.append(input.substr(last_pos, match.position() - last_pos));  // text before match

        const std::string var_name = match[1].str();
        const char* env_val = std::getenv(var_name.c_str());
        if (env_val) 
        {
            result.append(env_val);
        } else 
        {
            CMLMessage::error(__FILE__, __LINE__, 
              "Warning: Environment variable '", var_name, "' is not set. Leaving placeholder unchanged.\n");
            result.append(match[0].str());  // Keep the original "${VAR}"
            throw std::runtime_error("Missing environment variable: " + var_name);
        }
        last_pos = match.position() + match.length();
    }

    result.append(input.substr(last_pos));  // remaining text
    return result;
}

/*****************************************************************************
configure_from_definition_file
Purpose:(Used when the tests are specified in a single file with each row of
         data representing a unique test.)
*****************************************************************************/
void
UnitTestFramework::configure_from_definition_file()
{
  if (vars_file_name.empty() || data_file_name.empty()) {
    CMLMessage::fail(
    __FILE__,__LINE__,"invalid filename\n",
    "Check that vars_file_name AND data_file_name have been specified\n"
    "vars_file_name: ", vars_file_name, "\n"
    "data_file_name: ", data_file_name, "\n");
  }

  populate_variable_names();
  unsigned int num_vars = variables.size();


  // parse the data file
  std::ifstream data_file(data_file_name);
  if (!data_file) {
    CMLMessage::fail(
    __FILE__,__LINE__,"invalid filename\n",
    "Could not open data file, filename ", data_file_name, "\n");
  }
  unsigned int line_num = 0;
  unsigned int num_data_sets = 0;
  std::string line; // scratch variable
  while (std::getline(data_file, line)) {

    // if the line is empty, skip it.
    if (line.empty()) {
      continue;
    }
    // if the line begins with a !, treat the rest of the line as a title
    if (line.at(0) == '!') {
      // fill in the title array from the last recorded element to this one
      // with blanks.
      for (unsigned int ii = titles.size(); ii < num_data_sets; ii++) {
        titles.push_back("");
      }
      // Add this line (without the !) as the title and go on to the next
      // line
      titles.push_back(line.substr(1));
      continue;
    }

    std::string new_command_line;
    std::istringstream stream(line);
    unsigned int variable_ix = 0;

    // For each line in the stream:
    do {
      // Grab the words one at a time:
      std::string word;
      stream >> word;

      // Note - breaks on each white-space character, so words can be empty if
      // there are multiple adjacent whitespace characters.  If the word is
      // empty, just continue to the next.
      if (word.empty()) {
        continue;
      }

      // if the word begins with a #, treat the rest of the line as a comment
      // break out of this line.
      if (word.at(0) == '#') {
        break;
      }

      // do the replacement ${} here
      word = expand_env_variables(word);

      // Fail if the number of data points on this line exceeds the number of
      // variables
      if (variable_ix >= num_vars) {
        CMLMessage::fail(
        __FILE__,__LINE__,"Invalid data\n",
        "Data file ", data_file_name, " at line ", line_num,
        " contains more than ", num_vars, " data elements\n"
        "but there are only ", num_vars, " variables to populate\n");
      }

      // And if all sanity checks pass, add this data point to the command
      // line and increment the number of data points found on the line
      new_command_line.append( variables.at(variable_ix) + " = " + word + "; ");
      variable_ix++;

    } while (stream); // ends processing of this line

    // Fail if got the wrong number of data points (not including blank lines)
    // (already failed if too many, this fails if too few)
    if (variable_ix > 0 && variable_ix != num_vars) {
      CMLMessage::fail(
      __FILE__,__LINE__,"Invalid data\n",
      "Data file ", data_file_name, " at line ", line_num,
      " contains ", variable_ix, " data elements\n"
      "but there are ", num_vars, " variables to populate\n");
    }
    // else
    commands.push_back(new_command_line);
    num_data_sets++;
    line_num++;
  }
}

/*****************************************************************************
populate_linked_variables
Purpose:(Populates the linked_variables list, linking variable names with
         their respective data file names.
*****************************************************************************/
void
UnitTestFramework::populate_linked_variables()
{
  std::string line; // scratch variable

  // parse the variables file
  std::ifstream vars_file(linked_vars_file_name);
  if (!vars_file) {
    CMLMessage::fail(
    __FILE__,__LINE__,"invalid filename\n",
    "Could not open variables file, filename ", linked_vars_file_name, "\n");
  }
  while (std::getline(vars_file, line)) {
    std::istringstream stream(line);
    LinkedVars staging;
    int count = 0;

    do {
      std::string word;
      stream >> word;
      // Note - breaks on each white-space character, so words can be empty if
      // there are multiple adjacent whitespace characters.  If the word is
      // empty, just continue to the next.
      if (word.empty()) {
        continue;
      }
      // Break on a comment symbol
      if (word.at(0) == '#') {
        break;
      }
      if (count == 0) {
        staging.variable_name = word;
        count = 1;
      }
      else if (count == 1) {
        staging.filename = word;
        linked_variables.push_back(staging);
        count = 2;
      }
      else {
        CMLMessage::fail(
        __FILE__,__LINE__,"invalid file format\n",
        "More than 2 entries were detected on a line in file ", linked_vars_file_name, "\n",
         line, "\n");
      }
    } while (stream);

    if (count ==1) {
      CMLMessage::fail(
      __FILE__,__LINE__,"invalid file format\n",
      "Only 1 entry was detected on a line in file ", linked_vars_file_name, "\n",
       line, "\n");
    }
    // next line
  }
}

/*****************************************************************************
process_linked_variables
Purpose:(Produces the list of commands to be sent at runtime)
*****************************************************************************/
void
UnitTestFramework::process_linked_variables()
{
  commands.push_back("");// create 1 empty element

  for( std::list<LinkedVars>::iterator file_it = linked_variables.begin();
       file_it != linked_variables.end();
       ++file_it) {
    std::ifstream data_file( file_it->filename);
    if (!data_file) {
      CMLMessage::fail(
      __FILE__,__LINE__,"error opening file\n",
      "Error encountered opening file ", file_it->filename, "\n");
    }

    std::string first_command;
    std::list<std::string> subsequent_commands;

    // read data
    std::string line; // scratch variable
    while (std::getline(data_file, line)) {
      // if the line is empty, skip it.
      if (line.empty()) {
        continue;
      }
      // if the line begins with a !, consider it as a legacy title and skip it
      if (line.at(0) == '!') {
        CMLMessage::warn(
        __FILE__,__LINE__,"illegal character\n",
        "Skipping entry ", line, "; titles not suppoted in this format\n");
        continue;
      }

      // formulate commands
      // turn the line into a string-stream so it can be divided into words
      std::istringstream stream(line);
      unsigned int word_count = 0;
      // For each line in the stream:
      do {
        // Grab the words one at a time:
        std::string word;
        stream >> word;

        // Note - breaks on each white-space character, so words can be empty if
        // there are multiple adjacent whitespace characters.  If the word is
        // empty, just continue to the next.
        if (word.empty()) {
          continue;
        }

        // if the word begins with a #, treat the rest of the line as a comment
        // break out of this line.
        if (word.at(0) == '#') {
          break;
        }

        // Fail if the number of data points on this line exceeds 1
        if (word_count >= 1) {
          CMLMessage::fail(
          __FILE__,__LINE__,"Invalid data\n",
          "Data file ", file_it->filename, " has multiple words on one line:\n", line, "\n");
        }

        word_count++;
        // Use "word" to make a new command
        std::string new_command( file_it->variable_name);
        new_command.append( " = " + word + ";");
        if (first_command.empty()) {
          first_command = new_command;
        }
        else {
          subsequent_commands.push_back( new_command);
        }
      } while (stream); // ends processing of this line
    } // end processing of this file

    data_file.close();

    // insert new commands into command list
    for( std::list<std::string>::iterator comm_it = commands.begin();
         comm_it != commands.end();
         /*increment internally*/ ) {
      comm_it->append( first_command);
      ++comm_it;
      commands.insert(comm_it,
                      subsequent_commands.begin(),
                      subsequent_commands.end());
    }
  } // completed all variables
}

/*****************************************************************************
populate_variable_names
Purpose:(Populates the vector of variables from the file identified by the
         vars_file_name string)
*****************************************************************************/
void
UnitTestFramework::populate_variable_names()
{
  std::string line; // scratch variable

  // parse the variables file
  std::ifstream vars_file(vars_file_name);
  if (!vars_file) {
    CMLMessage::fail(
    __FILE__,__LINE__,"invalid filename\n",
    "Could not open variables file, filename ", vars_file_name, "\n");
  }
  while (std::getline(vars_file, line)) {
    std::istringstream stream(line);

    do {
      std::string word;
      stream >> word;
      // Note - breaks on each white-space character, so words can be empty if
      // there are multiple adjacent whitespace characters.  If the word is
      // empty, just continue to the next.
      if (!word.empty()) {
        variables.push_back(word);
      }
    } while (stream);
  }
}

/*****************************************************************************
update_sweeps
Purpose:(update method when sweeps are in effect.)
Assumptions:
     (Sweeps run all combinations of data for all variables in the framework.
      Be careful adding multiple variables with large data sets, the test
      grows rapidly.
      e.g. 4 variables each with 10 values produces 10,000 tests -- not 10!)
*****************************************************************************/
void
UnitTestFramework::update_sweeps()
{
  if (sweeps_complete) {
    if (warning_on_sim_end) {
      cycle_overruns++;
      if (cycle_overruns > cycle_overruns_limit-1) {// -1 because first overrun
                                                    // is not counted
        CMLMessage::warn(
        __FILE__,__LINE__,"\nInvalid sim-stop specification\n",
        "Completed sweep of all variables before sim shut down.\n"
        "Cycling redundantly until sim ends.\n");
        warning_on_sim_end = false;
      }
    }
    return;
  }

  bool sweep_complete_ = true;
  for( std::list<SweepSet>::iterator it = sweeps.begin();
       it != sweeps.end();
       ++it)
  {
    // increment_sweep returns true if the sweep of that variable is complete
    sweep_complete_ = it->increment_sweep();
    // if this variable is still sweeping, don't need to go on to the next one.
    if (!sweep_complete_) break;
  }
  // if sweep_complete_ is true at this point, that means that all sweep sets
  // finished on this cycle.  In that case, the whole unit-test is complete,
  // and the data is complete if it has not been tagged to re-cycle.
  sweeps_complete = sweep_complete_ && !cycle_data;
}

/*****************************************************************************
update_file
Purpose:(reads in the next data set fom the file.)
*****************************************************************************/
void
UnitTestFramework::update_file()
{
  // if the commands expire while the sim is still going, drop an alert
  if (commands.empty()) {
    if (warning_on_sim_end) {
      cycle_overruns++;
      if (cycle_overruns > cycle_overruns_limit) {
        CMLMessage::warn(
        __FILE__,__LINE__,"\nInvalid sim-stop specification\n",
        "Ran out of lines of data before sim shut down.\n"
        "Cycling redundantly until sim ends.\n");
        warning_on_sim_end = false;
      }
    }
    return;
  }

  // If the user has specified titles in the data file, print them out
  if ( !titles.empty()) {
    if (!titles.front().empty()) {
      std::cout << titles.front() << std::endl;
    }
    if (cycle_data) {
      titles.push_back(titles.front());
    }
    titles.pop_front();
  }

  // Reads and processes the data and variables files as if they were
  //   in a Python input file.  Locks the files automatically
  ip_parse(commands.front().c_str());

  if (cycle_data) {
    commands.push_back(commands.front());
  }
  commands.pop_front();
  return;
}
