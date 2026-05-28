/*******************************************************************************
PURPOSE:
   (Define the dynamic mass group.
   (The DynamicMassGroup is the grouping of DynamicMassBody objects for the
   purpose of corralling the updates to all mass udpates for any vehicle, and
   the subsequent update to the entire mass tree.  Single-entity
   DynamicMassBody may be added to a group.)

ASSUMPTIONS AND LIMITATIONS:
   ((Assumption 1: The DynamicMassBody will be initialized in the same way as a
                   regular jeod::MassBody, using the MassBodyInit jeod::BodyAction)
    (Assumption 2: Following MassBodyInit initialization, the
                   initialize_dyn_mass method sets the nominal properties and
                   initializes the dynamic properties.)
    (Assumption 3: changes to the dynamic mass are made by changing the
                   mass_consumed_step variable in the dynamic properties.  This
                   value is set to zero at each update cycle, so it is safe to
                   increment it in the event that there may be more than one
                   affecting consideration)
    (Assumption 4: unless the interpolation is used, the position of the cg
                   remains constant and the core-properties inertia tensor scales
                   with mass.)
    (Assumption 5: if the interpolation is being used, it is assumed that the user
                   correctly loads the interpolation tables.)
    (Assumption 6: A group is a collection of inseperable dynamic masses.
                   Detachment of two DynamicMassBody from the same group into
                   two separate vehicle will cause the model to fail.)
    (Limitation 1: The update_mass method updates the core properties and sets
                   the flag for the tree update, but does not call
                   update_mass_properties on the root body.  That is left to an
                   independent call or to the DynamicMassGroup update method.)
   )

LIBRARY DEPENDENCIES:
   ((../src/dynamic_mass_group.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (March 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
 ******************************************************************************/

#ifndef ANTARES_DYN_MASS_GROUP_HH
#define ANTARES_DYN_MASS_GROUP_HH

#include<vector>
#include<list>
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh" // for moving-mass option
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"

#include "dynamic_mass_body.hh"
#include "dynamic_mass_string.hh"

class DynamicMassGroup : public SubscriptionBase {
 public:
  // Inputs:
  bool delta_mass_is_redistributed; /* (--)
      Flag indicating the dynamic behavior of the vehicle.  If the delta-mass
      is redistributed, the CM-motion is conserved, and the structural frame
      moves relative to the body-frame.  If the mass is lost (or gained) from
      an external source, the structural frame motion is conserved, and the
      CM must be repositioned inside the structural frame.  This flag controls
      the preferred behavior. Default: false (mass deltas are considered
      sink/source to external)*/

  std::string name;     /* (--)
      Group name, optional setting for message clarity and debugging only.*/

  unsigned int countdown_reset; /* (count)
       Sets the frequency with which the root body test is performed.*/

 protected:
  bool flow_warn_msg_shown; /* (--)
       Flag to limit showing the warning message in series_flow() to a
       single instance.*/

  bool needs_tree_update; /* (--)
       Flag to indicate whether any part of the dynamic masses have changed.
       If they have, the tree needs updating.
       NOTE -- this is a class member because it would typically default to
       false until a mass-change is detected; however, at initialization it
       must default to true to force a string-update. */

  double total_mass;          /* (kg)
       Total mass from all bodies in the group. */

  double total_initial_mass;  /* (kg)
       Total mass from all bodies in the group at initialization of the
       group. */

  double total_consumed_mass; /* (kg)
       Total mass consumed from all bodies in the group. */

  unsigned int countdown_to_root_test; /* (count)
       Counts down to testing the legitimacy of the root body */

  unsigned int num_dyn_masses; /* (--)
       Size of the dyn_masses vector.*/

  jeod::MassBody * root_body_ptr; /* (--)
       Pointer to the root of the mass tree.*/

  std::vector< DynamicMassBody *> dyn_masses; /* (--)
       Pointers to the dynamic masses in the group.  Using a vector here to
       support the series_flow method.*/
  std::list <DynamicMassString *> mass_strings; /* (--)
       Pointers to the strings of connected mass bodies. */


 public:
  DynamicMassGroup(void);
  void add_mass_to_group( DynamicMassBody *mass);
  void add_string_to_group( DynamicMassString *string);
  virtual void initialize();
  void update_group_mass();
  void series_flow(unsigned int index1,
                   unsigned int index2);
  void initiate_dry_mass_config();
  void revert_dry_mass_config();

  bool is_body_in_group( DynamicMassBody * body);
  bool is_string_in_group( DynamicMassString * body);
 protected:
  void test_root_body();
  void add_mass_to_group_internal( DynamicMassBody *mass,
                                   bool send_err_msg);

 private:
  // To disable the copy/assignment operations
  DynamicMassGroup (const DynamicMassGroup&);
  DynamicMassGroup& operator = (const DynamicMassGroup&);
};
#endif
