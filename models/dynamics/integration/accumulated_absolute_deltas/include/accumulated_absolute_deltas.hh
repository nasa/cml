/*******************************TRICK HEADER******************************
PURPOSE: (
    Provides a means of accumulating the absolute-value of changes to some
    time-dependent variable.  While obtaining the net change in a variable
    is a straightforward task (the net change is just how much the variable
    has changed), the total change must be accumulated over time.
    This model takes the absolute-value of any changes to a variable
    and accumulates that value.

    An example of where this might be useful - some bi-directional effector
    being used to control a position or attitude to a fixed value when subject
    to oscillatory perturbations.  The net effect of this effector may
    be close to zero, but depending on the controller, it may be working
    excessively back-and-forth to maintain that zero.
    The accumulated absolute-value will be indicative of how much the
    effector has actually been used.)

ASSUMPTIONS: (
   (The integrated variable that this is paired with is represented as
    type "double")
   (The integrated variable that this is paired with is a single element or
    a single array.  If an array, the array can be of any size, but the model
    cannot handle multiple variables in one instance.))

LIBRARY DEPENDENCY:
   ((../src/accumulated_absolute_deltas.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2017) (Antares) (new)))
**********************************************************************/

#ifndef ACCUMULATED_ABSOLUTE_DELTAS_HH
#define ACCUMULATED_ABSOLUTE_DELTAS_HH

#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

class AccumulatedAbsoluteDeltas : public SubscriptionBase
{
 protected:
  const double * variable; /* (--) Time-dependent variable to accumulate
       changes over time */
  const unsigned int size; /* (--) Size of the variable */
  double * old_variable; /* (--) Copy of the variable containing values
       one frame behind the variable */

 public:
  double * accumulated_deltas; /* (--) Difference between variable values
       and old_variable values */
  AccumulatedAbsoluteDeltas(  const double * variable_,
                              const unsigned int size_);
  virtual ~AccumulatedAbsoluteDeltas();

  virtual void initialize();
  void reset();
  void update();
 protected:
  virtual void activate();
 private:
  // Not implemented
  AccumulatedAbsoluteDeltas ( const AccumulatedAbsoluteDeltas &);
  AccumulatedAbsoluteDeltas & operator= ( const AccumulatedAbsoluteDeltas &);
};
#endif