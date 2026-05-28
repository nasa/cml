/*******************************TRICK HEADER******************************
PURPOSE: (A basic data structure from which values will be logged)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Jan 2024) (ANTARES) (initial)))
**********************************************************************/
#ifndef CML_SUMMARY_LOGGING_VERIFICATION_TEST_STRUC_HH
#define CML_SUMMARY_LOGGING_VERIFICATION_TEST_STRUC_HH

#include "cml/models/vehicle_management/compound_events/include/compound_event.hh"
struct VerifTestStruc {
  double dyn_time;/* (s) simulation-record of dynamic-time.*/
  double      x1; /* (kg) x1 */
  double      x2; /* (N) x2 */
  double      x_vec[3]; /* (m) x_vec */
  double      x_arr[2][2]; /* (--) */
  double      x_arr5[2][3][4][3][2]; /* (--) */
  int         i1; // (count)
  bool        b1; // (--)
  std::string s1; // (--)
  enum TestEnum {
    Enum1 = 1,
    Enum3 = 3   } e1; // (1)
  CompoundEvent event; /* (--)
    A generic external compound-event used to evaluate one of
    the recording-conditions. */
  EventTrigger<int> trigger_i1_ne_n9;


  VerifTestStruc()
    :
    dyn_time(0.0),
    x1(1.2345),
    x2(0.0),
//    x_vec{ 2.1, 3.2, 4.3},
//    x_arr{{11.2, 12.3},{21.4, 22.5}},
    i1(-2),
    b1(true),
    s1("test"),
    e1(Enum3),
    event(dyn_time),
    trigger_i1_ne_n9( dyn_time,
                      "sim event")
  {
    x_vec[0] = 2.1;
    x_vec[1] = 2.2;
    x_vec[2] = 2.3;

    x_arr[0][0] = 3.11;
    x_arr[0][1] = 3.12;
    x_arr[1][0] = 3.21;
    x_arr[1][1] = 3.22;

    for (int ii0 = 0; ii0 < 2; ++ii0) {
      for (int ii1 = 0; ii1 < 3; ++ii1) {
        for (int ii2 = 0; ii2 < 4; ++ii2) {
          for (int ii3 = 0; ii3 < 3; ++ii3) {
            for (int ii4 = 0; ii4 < 2; ++ii4) {
              x_arr5[ii0][ii1][ii2][ii3][ii4] =
                    4+ ii0/10. + ii1/100. + ii2/1000. + ii3/10000. + ii4/100000.;
    }}}}}



    trigger_i1_ne_n9.set_watch( i1, -9);
    trigger_i1_ne_n9.comparison_logic = EventTriggerBase::NE;
    trigger_i1_ne_n9.message = "i1 != -9 trigger detected.";
    event.action_triggers.add_trigger( trigger_i1_ne_n9);
    event.action_triggers.message = "Sim-data event triggered.";
    event.add_self_to_manager_active_list = false;
    event.initialize(nullptr);

  }
 private:
  VerifTestStruc( const VerifTestStruc&);
  VerifTestStruc& operator=( const VerifTestStruc&);
};
#endif
