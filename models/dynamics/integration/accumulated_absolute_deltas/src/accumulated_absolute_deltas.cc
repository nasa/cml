/*******************************TRICK HEADER******************************
PURPOSE: (Provides an accumulation of an integrated value, accumulating
          the absolute value of that variable rather than the variable
          itself.  This gives the total change rather than the net change
          -- which can be easily obtained from the variable itself.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Apr 2017) (Antares) (new)))
**********************************************************************/

#include <cstddef> // NULL
#include <cmath>   // abs
#include <new>

#include "../include/accumulated_absolute_deltas.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
AccumulatedAbsoluteDeltas::AccumulatedAbsoluteDeltas(
     const double * variable_,
     const unsigned int size_)
    :
    variable(variable_),
    size(size_),
    old_variable(NULL),
    accumulated_deltas(NULL)
{
  subscribe_name = "AccumulatedAbsoluteDeltas:";

  if (variable == NULL) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid construction\n",
    "The variable being accumulated in NULL.\n");
  }
  if (size == 0) {
    CMLMessage::fail(
    __FILE__,__LINE__,"Invalid construction\n",
    "The size of the array must be positive.\n"
    "For a scalar, use size = 1.\n");
  }

  accumulated_deltas = new double[size];
  old_variable = new double[size];
}

/*****************************************************************************
destructor
*****************************************************************************/
AccumulatedAbsoluteDeltas::~AccumulatedAbsoluteDeltas()
{
  delete[] old_variable;
  delete[] accumulated_deltas;
}

/*****************************************************************************
initialize
Purpose:(Initializes the old-variable data)
*****************************************************************************/
void
AccumulatedAbsoluteDeltas::initialize()
{
  if (!enabled) {
    return;
  }

  reset();
  SubscriptionBase::initialize();
}


/*****************************************************************************
activate
Purpose:(activates the model)
*****************************************************************************/
void
AccumulatedAbsoluteDeltas::activate()
{
  active = true;
  reset();
}


/*****************************************************************************
reset
Purpose:(resets the accumulated value and the starting values)
*****************************************************************************/
void
AccumulatedAbsoluteDeltas::reset()
{
  for (unsigned int  ii = 0; ii < size; ++ii) {
    old_variable[ii] = variable[ii];
    accumulated_deltas[ii] = 0.0;
  }
}

/*****************************************************************************
update
Purpose:(accumulates the deltas since the last update.)
*****************************************************************************/
void
AccumulatedAbsoluteDeltas::update()
{
  if (!active) {
    return;
  }
  for (unsigned int ii = 0; ii < size; ++ii) {
    accumulated_deltas[ii] += std::abs( variable[ii] - old_variable[ii]);
    old_variable[ii] = variable[ii];
  }
}