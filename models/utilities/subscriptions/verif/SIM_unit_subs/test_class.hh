/*******************************TRICK HEADER******************************
PURPOSE:
  (A derivative class that deliberately fails activation to test
  the response of the base class.)

PROGRAMMERS:
  (((Gary Turner) (OSR) (Sep 2021) (Antares) (initial)))
***********************************************************************/

#ifndef CML_SUBSCRIPTION_TEST_CLASS_HH
#define CML_SUBSCRIPTION_TEST_CLASS_HH

#include <iostream>
#include "../../include/subscriptions.hh"

class SubscriptionTestClass : public SubscriptionBase
{
  protected:
  void activate() {
    std::cout << "******** ACTIVATION FAILS ********\n\n";
    }
};
#endif
