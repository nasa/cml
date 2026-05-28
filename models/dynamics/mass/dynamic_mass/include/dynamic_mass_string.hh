/*******************************************************************************
PURPOSE:
   (Define the dynamic mass string.
    The DynamicMassString is the extension of the DynamicMassBody concept.
    It represents a collection of massive bodies from/to which mass may be
    removed/added.)

ASSUMPTIONS AND LIMITATIONS:
   ((Assumption 1: As long as the DynamicMassBody elements are flagged as
                   available, mass will be added removed equally from all
                   elements.)
   )

LIBRARY DEPENDENCIES:
   ((../src/dynamic_mass_string.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (July 2014)
                      (New implementation of dynamic mass for JEOD 2.x))
    ((Gary Turner, Bingquan Wang) (OSR) (January 2018)
                      (Refactor to utilize new CML capabilities))
   )
 ******************************************************************************/

#ifndef ANTARES_DYN_MASS_STRING_HH
#define ANTARES_DYN_MASS_STRING_HH

#include "dynamic_mass_body.hh"
#include "dynamic_mass_body_properties.hh"

class DynamicMassString  : public DynamicMassBodyPropertiesInterface
{
public:
 // NOTE:
 // mass_consumed_step and
 // consumable_mass
 // are provided via inheritance from DynamicMassBodyPropertiesInterface.
 // This interface is shared with DynamicMassBody's dynamic_properties.
 //
 // double mass_consumed_step  (kg)
 //  The mass that has been consumed since last update as a result of connection
 //  to this string exclusively.  Note that this may be different from the sum
 //  of the mass consumption of all bodies to which this string is attached if
 //  those bodies are, in turn, connected to other strings.
 // double consumable_mass  (kg)
 //  The mass remaining to be potentially consumed.  >= 0.

   bool flow_down; /* (--)
       True: deplete body[0] to exhaustion, then body[1] etc.
       False: deplete mass from all bodies equally (default).*/

protected:
   double mass_consumed;         /* (kg)
      The mass that has been consumed exclusively by demands on this string.*/

   std::list< DynamicMassBody *> body_collection; /* (--)
      The dynamic masses in the string*/
   std::list< DynamicMassBody *> available_bodies; /* (--)
      The dynamic masses in the string in the order in which they will be
      processed.  This changes as bodies exhaust their consumable-mass.*/

private:
   bool string_in_group; /* (--)
      Defaults to false and is switched to true when the string has been
      successfully added to a group.  Bya dding the string to the group,
      all of the masses in the string AT THE TIME THE STRING WAS ADDED TO
      THE GROUP will also be added to the group.*/
public:
   DynamicMassString();
   void initialize();
   void distribute_mass_consumption();
   void reenable_bodies();
   void add_mass_to_string(DynamicMassBody * mass);
   void update();

   std::list<DynamicMassBody*> & get_body_collection() {return body_collection;}
   size_t get_body_collection_size() { return body_collection.size(); }

   void set_string_in_group() {string_in_group = true;}

private:
   // To prevent the copy/assignment operations
   DynamicMassString (const DynamicMassString&);
   DynamicMassString& operator = (const DynamicMassString&);
};
#endif
