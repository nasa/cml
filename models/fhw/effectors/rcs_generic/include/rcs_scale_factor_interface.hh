/*******************************************************************************
PURPOSE:
  (Structure for data shared between scale factor models and jet-specific
   scale-factor data)

PROGRAMMERS:
  (((Daniel Ghan) (OSR) (Mar 2020) (ANTARES) (Initial implementation)))
******************************************************************************/

#ifndef RCS_SCALE_FACTORS_INTERFACE
#define RCS_SCALE_FACTORS_INTERFACE

/*****************************************************************************
RcsScaleFactorInterfaceJet
Purpose:(Jet-specific data for RcsScaleFactorInterface)
*****************************************************************************/
class RcsScaleFactorInterfaceJet
{
 public:
  bool rcs_valve_open; /* (--) true:  Jet valve is open
                               false: Jet valve is closed */
  double rcs_thrusting_start_time; /* (s) Sim-time when the jet valve opened */
  double rcs_thrusting_stop_time;  /* (s) Sim-time when the jet valve closed */

  RcsScaleFactorInterfaceJet()
  :
    rcs_valve_open(false),
    rcs_thrusting_start_time(-1.0),
    rcs_thrusting_stop_time(-1.0)
  {}
};

/*****************************************************************************
RcsScaleFactorInterface
Purpose:(Structure for sharing data among scale-factor models)
*****************************************************************************/
class RcsScaleFactorInterface
{
 public:
  const unsigned int num_jets; /* (--) Number of jets in the system */

  double valve_rise_time;  /* (s)
        Time required for the solenoid to generate a magnetic field strong
        enough to open the valve */
  double valve_decay_time; /* (s)
        Time required for the solenoid's magnetic field to weaken and allow
        the valve to close */

  // This is a pure virtual class. Any instantiation is expected to have a
  // a vector of something that inherits from RcsScaleFactorInterfaceJet
  // (with a length of num_jets). get_jet here is a way to give generic
  // scale-factor models access to the members of RcsScaleFactorInterfaceJet.
  virtual RcsScaleFactorInterfaceJet* get_jet(unsigned int id) = 0;

  RcsScaleFactorInterface( const unsigned int num_jets_)
  :
    num_jets(num_jets_),
    valve_rise_time(0.0),
    valve_decay_time(0.0)
  {}

  virtual ~RcsScaleFactorInterface() {};

 private:
  RcsScaleFactorInterface(const RcsScaleFactorInterface&);
  RcsScaleFactorInterface & operator= (const RcsScaleFactorInterface&);
};
#endif
