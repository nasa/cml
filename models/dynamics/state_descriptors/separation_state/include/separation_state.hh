/******************************** TRICK HEADER **********************************
PURPOSE:
   (Defines the class that is used for computing relative states during separation.)

LIBRARY DEPENDENCIES:
   ((../src/separation_state.cc))

PROGRAMMERS:
   (((Gary Turner) (OSR) (April 2014) (Antares) (Replaces SEPKIN))
    ((Gary Turner) (OSR) (December 2014) (Antares)
                           (Greater protection, added subscription concept))
    ((Bingquan Wang) (OSR) (May 2017) (Antares)
                           (cleaned up the code per its IV&V code review))
   )

********************************************************************************/

#ifndef CML_SEPARATION_STATE_HH
#define CML_SEPARATION_STATE_HH

#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "jeod/models/utils/orientation/include/orientation.hh"
#include "jeod/models/dynamics/dyn_body/include/body_ref_frame.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "jeod/models/dynamics/dyn_manager/include/dyn_manager.hh"
#include "cml/models/utilities/subscriptions/include/subscriptions.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"


class SeparationState : public SubscriptionBase
{
 public:
   // The relative state is the state of "subject" relative to and
   // expressed in "source".  "subject" and "source" are protected.
   bool generate_euler_angles; /* (--)
       Flag indicating whether it is necessary to compute the Euler angles
       from frame to frame.  Default: True. */
   jeod::RefFrameState  rel_state; /* (--)
       the complete relative state between source and subject. */
   jeod::Orientation    rel_orientation; /* (--)
       the relative orientation of subject wrt source. Its purpose is to
       calculate the Euler angles, which are located in
       rel_orientation.euler_angles. If generate_euler_angles == false, the
       entire object will not be updated. The relative transformation matrix
       and quaternion are in rel_state.rot.T_parent_this and
       rel_state.rot.Q_parent_this, respectively. */
   double separation_distance; /* (m) magnitude of rel_state.trans.position. */
   double separation_rate; /* (m/s)
     Rate at which the separation distance changes.
     The component of the relative velocity along the line
     separating the two points.*/

 protected:
   bool frames_subscribed; /* (--)
       The jeod::DynManager has subscriptions to the necessary frames on account
       of this instance.  Used to determine whether to unsubscribe. */

   // The state will be computed between two reference frames.
   // These are identified as "source" and "subject"
   jeod::RefFrame * source;  /* (--) the root frame of the relative state,
                                     the "from" side of the relation. */
   jeod::BodyRefFrame * subject;  /* (--) the other of the relative state, the "to"
                                          side of the relation. */

 public:
   SeparationState (void);
   explicit SeparationState (std::string name);
   virtual ~SeparationState(){};

   void initialize( jeod::DynManager & dyn_manager_in,
                    jeod::DynBody & source_body,
                    jeod::DynBody & subject_body,
                    std::string source_name,
                    std::string subject_name) // DEPRECATED
   { (void)dyn_manager_in;
     initialize(source_body, subject_body, source_name, subject_name);}

   void initialize( jeod::DynManager & dyn_manager_in,
                    jeod::RefFrame     & source_frame,
                    jeod::BodyRefFrame & subject_frame) // DEPRECATED
   { (void)dyn_manager_in;
     initialize(source_frame, subject_frame);}


   void initialize( jeod::RefFrame     & source_frame,
                    jeod::BodyRefFrame & subject_frame);
   void initialize( jeod::DynBody      & source_body,
                    jeod::BodyRefFrame & subject_frame,
                    std::string    source_name = "");
   void initialize( jeod::RefFrame     & source_frame,
                    jeod::DynBody      & subject_body,
                    std::string    subject_name = "");
   void initialize( jeod::DynBody      & source_body,
                    jeod::DynBody      & subject_body,
                    std::string    source_name = "",
                    std::string    subject_name = "");
   virtual void update( );
   jeod::RefFrame* get_source() {return source;};
   jeod::BodyRefFrame* get_subject() {return subject;};

 protected:
   jeod::BodyRefFrame *  initialize_find_frame (jeod::DynBody &,
                                                std::string,
                                                std::string);
   virtual void activate();
   virtual void deactivate();

 private:
   // Not implemented:
   SeparationState (const SeparationState&);
   SeparationState& operator = (const SeparationState&);

};
#endif
