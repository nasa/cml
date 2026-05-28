/******************************** TRICK HEADER **********************************
PURPOSE:
   (Header and structure definitions for simple 1 - cosine gust model.)

LIBRARY DEPENDENCY:
    ((../src/gust.cc))

PROGRAMMERS:
   (((Jon Berndt) (ESCG/Jacobs) (February 2008) (ANTARES) (Initial version))
    ((Brent Caughron) (Odyssey) (Sept. 2017) (Antares) (IV&V Code Review)))
********************************************************************************/
#ifndef CML_GUST_MODEL_HH
#define CML_GUST_MODEL_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"


class GustModel
{
 protected:
  // External references:
  const double & time;        /* (s)  Reference to simulation dynamic-time*/

  // Internal variables:
  double phase_angle;        /* (rad)
    Intermediate variable for determining the gust speed */
  double dir_unit_vector[3];  /* (--)
    Defines the unit vector of the direction the model is gusting */
  double elapsed_time;        /* (s)
    The elapsed time after the model has been turned on and is calculating
    the gust velocity vector */
  double start_time;          /* (s)
    The starting time for the gust */
  bool   active;              /* (--)
    The flag that determines if the model has been turned on or not */
  bool   period_message_sent; /* (--)
    The flag the tells the model if a warning about the period has been given
    to the user or not */

 public:
  enum GustFrame
  {
    TD_FRAME   = 0,
    BODY_FRAME = 1
  }  gust_frame;     /* (--)
    Identifies the frame used in the direction term, in the atmost_exec model*/
  double direction[3];      /* (--)  Defines a vector direction for the gust */
  double maximum_magnitude; /* (m/s) Maximum gust speed */
  double magnitude;         /* (m/s) Actual gust speed */
  double gust_vector[3];    /* (m/s)
    Wind gust vector in frame spec. by gust_frame */
  double period;            /* (s)   The period of the gust waveform (1/Hz) */
  bool   start;             /* (--)
    Flag to tell the model to begin gusting.
    Setting start to true while inactive will activate the model.
    Setting start to true while active will result in the gust profile
    cycling again after the current gust has completed.*/

  explicit GustModel(const double & time_in);
  virtual ~GustModel(){};

  void   activate();
  void   update();
  void   deactivate();
  bool   is_active(){return active;}

 private:
  GustModel (const GustModel&) = delete;
  GustModel & operator = (const GustModel&) = delete;
};
#endif
