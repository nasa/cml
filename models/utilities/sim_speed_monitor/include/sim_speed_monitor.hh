/*******************************TRICK HEADER******************************
 PURPOSE: (Computes the simulation speed at regular intervals during
           execution to support identification of problem areas.)

 PROGRAMMERS:
   (((Gary Turner) (OSR) (Sep 2022) (Antares) (initial)))
***********************************************************************/

#ifndef ANTARES_SIM_SPEED_MONITOR_HH
#define ANTARES_SIM_SPEED_MONITOR_HH

#include <chrono>

#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/math_utils/include/math_utils.hh"

class SimSpeedMonitor : public SubscriptionBase {
 protected:
  #ifndef SWIG
  std::chrono::steady_clock::time_point cycle_time;
  std::chrono::duration<double> delta_t;
  #endif
 public:
  double sim_speed;

  SimSpeedMonitor(){};

  virtual ~SimSpeedMonitor(){};

  // Note -- model is self-initializing, this gets called on the first pass
  // through calculate_rate(...)
  void initialize() {
    cycle_time = std::chrono::steady_clock::now();
    SubscriptionBase::initialize();
  }

  // class methods are not callable from the input file.
  #ifndef SWIG
  double get_dt() {return delta_t.count();}

  void calculate_rate(double sim_dt)
  {
    if (!active && !sub_pending) return;

    if (!initialized) {
      // first pass, record the current time but do not try to generate a speed
      // on this pass.
      initialize();
      return;
    }

    std::chrono::steady_clock::time_point new_time =
                                               std::chrono::steady_clock::now();
    delta_t =
              std::chrono::duration_cast<std::chrono::duration<double> >
                                                    (new_time - cycle_time);
    cycle_time = new_time;
    sim_speed = MathUtils::divide_protected( sim_dt,
                                             delta_t.count(),
                                             0,
                                             false);
  }
  #endif

 private: // and undefined:
  SimSpeedMonitor (const SimSpeedMonitor&);
  SimSpeedMonitor& operator = (const SimSpeedMonitor&);
};
#endif
