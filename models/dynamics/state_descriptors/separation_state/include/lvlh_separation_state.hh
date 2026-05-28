/******************************** TRICK HEADER **********************************

PURPOSE:
   (Defines the class that is used for computing relative states between two
   bodies, and expressing that in the LVLH frame of one of them.)

LIBRARY DEPENDENCIES:
   ((../src/lvlh_separation_state.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (October 2020) (Antares) (new)))

********************************************************************************/

#ifndef CML_LVLH_SEPARATION_STATE_HH
#define CML_LVLH_SEPARATION_STATE_HH

#include "jeod/models/utils/lvlh_frame/include/lvlh_frame.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"
#include "separation_state.hh"

class LvlhSeparationState : public SeparationState
{
 public:
  jeod::LvlhFrame lvlh; /* (--)
     JEOD construct that provides the LVLH frame, including placing this frame
     in the ref-frame tree and fully updating its state.
     Note that in some instances of LvlhSeparationState, it may be necessary
     to use another pre-existing instance of LvlhFrame to avoid having two
     identical frames. In such a situation, this instance does not get used.
     The reference lvlh_ref always accesses the correct instance of LvlhFrame.*/

 protected:
   jeod::RefFrame * planet_centered_inertial; /* (--)
         pointer to the planet-center frame used for updating the LVLH frame.
         The pointer is used for subscription management.*/
   jeod::BodyRefFrame * lvlh_origin_frame; /* (--)
         pointer to the frame used for the origin of the LVLH frame, which is
         used for updating the LVLH frame.
         The pointer is used for subscription management.*/
  jeod::LvlhFrame & lvlh_ref; /* (--)
     Reference to the LVLH frame  In the default configuration, this simply
     references the LvlhFrame instance, "lvlh", but may reference an external
     LvlhFrame instance if one already exists. */
  const bool using_external_lvlh; /* (--)
     Flag indicating whether we are using the internal frame (lvlh), or an
     external LVLH frame.*/

 public:
  LvlhSeparationState();
  explicit LvlhSeparationState(jeod::LvlhFrame & lvlh);
  virtual ~LvlhSeparationState();

  void initialize ( jeod::DynManager & dyn_manager,
                    std::string planet_name,
                    jeod::DynBody   & subject_body,
                    jeod::DynBody   & source_body,
                    std::string subject_name = "",
                    std::string source_name = "");

  void initialize ( jeod::DynManager   & dyn_manager,
                    jeod::BasePlanet   & planet,
                    jeod::BodyRefFrame & subject_frame,
                    jeod::BodyRefFrame & source_frame);

  void initialize ( jeod::DynManager   & dyn_manager,
                    jeod::BodyRefFrame & subject_frame);

  void update() override;

 protected:
  void activate() override;
  void deactivate() override;

 private:
   // Not implemented:
   LvlhSeparationState (const LvlhSeparationState&);
   LvlhSeparationState& operator = (const LvlhSeparationState&);
};
#endif

