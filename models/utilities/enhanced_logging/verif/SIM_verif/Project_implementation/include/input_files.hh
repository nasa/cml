/********************************* TRICK HEADER ********************************
PURPOSE: (
  Provides an implementation of the JIT-input files that can be compiled
  directly into the sim for purposes of obtaining code-coverage metrics)

LIBRARY DEPENDENCY:
   ((../src/input_files.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (June 2024) (ANTARES) (initial)))
*******************************************************************************/
#ifndef ENHANCED_LOGGING_MANAGER_PROJECT_INPUTS_HH
#define ENHANCED_LOGGING_MANAGER_PROJECT_INPUTS_HH

#include "project_logging_manager.hh"
#include "../../include/data_struc.hh"

struct ProjectInputFiles
{
 private:
  VerifTestStruc & sim_data; /* (--)
    Reference to the sim's test-data structure.*/
  ProjectLoggingManager & logging_manager; /* (--)
    Reference to the logging-manager.*/

 public:
  ProjectInputFiles (VerifTestStruc &,
                     ProjectLoggingManager &);
  virtual ~ProjectInputFiles(){};

  void run_1a_predefined();
  void run_1b_modified_predefined();
  void run_1c_new_group();
  void run_2_two_groups();
  void run_3_group_specs();
  void run_4_arrays();
  void run_5_partial_arrays();

 private:
  ProjectInputFiles( const ProjectInputFiles&);
  ProjectInputFiles& operator=( const ProjectInputFiles&);
};
#endif
