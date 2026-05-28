/*******************************TRICK HEADER******************************
PURPOSE: (Provides an extension that computes the relative position
          between one or more points fixed to the ground and one or more points
          fixed to a vehicle.)

ASSUMPTIONS / LIMITATIONS:
  ((Each instance of PointToPointManager deals with 1 planetary body and 1
    DynBody.  It can handle multiple points on each body, but cannot handle
    relative positions between a vehicle and Earth and Moon, or between two
    vehicles and Earth)
   (This is intended to be used only as a submodel of extended planetary
    derived state; it has no independent active or subscription management, it
    is assumed that extended planetary derived state takes care of its
    subscriptions to its submodels.))

LIBRARY DEPENDENCY:
   ((../src/point_to_point.cc))

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2021) (Antares) (initial)))
***********************************************************************/
#ifndef EXT_PLANET_DER_STATE_PT_TO_PT_HH
#define EXT_PLANET_DER_STATE_PT_TO_PT_HH

#include "jeod/models/utils/planet_fixed/planet_fixed_posn/include/planet_fixed_posn.hh"
#include "jeod/models/utils/ref_frames/include/ref_frame_state.hh"
#include "jeod/models/dynamics/dyn_body/include/dyn_body.hh"
#include "cml/models/utilities/cml_message/include/cml_message.hh"

#include <list>
#include <string>


/*****************************************************************************
PointToPointElement
Purpose:
  Provides the point on one end of the relative position vector.
*****************************************************************************/
class PointToPointElement
{
 public:
  double position[3]; /* (m)
    This 3-array serves 2 purposes depending on which type of point this
    instance represents:
    - describes a point on the vehicle, expressed in the structural frame of
      the vehicle (for a vehicle-based point)
    - describes a point on the planet, expressed in the pfix frame of the
      planet (for a planet-based point)*/

  std::string name; /* (--) the name of the point. */
  PointToPointElement( std::string name,
                       double      position[3]);
  PointToPointElement( const PointToPointElement & orig);
  virtual ~PointToPointElement(){};
  // Accept default operator=; it isn't used in the implementation.
};



/*****************************************************************************
PointToPointPosition
Purpose:
  Describes the relative position between two PointToPointElements.
*****************************************************************************/
class PointToPointPosition
{
  friend class PointToPointManager;

 public:
  double position[3]; /* (m)
    This 3-array describes the relative position of the vehicle-point from the
      planet-point, expressed in the pfix frame of the planet.*/

 protected:
  double v_pos[3]; /* (m)
    Copy of the position from the PointToPointElement representing the point on
    the vehicle.*/
  double p_pos[3]; /* (m)
    Copy of the position from the PointToPointElement representing the point on
    the planet.*/
  std::string v_name; /* (--) Name of the vehicle-based point that terminates
                              the relative-position vector.*/
  std::string p_name; /* (--) Name of the planet-based point from which the
                              relative-position vector originates.*/

 public:
  PointToPointPosition( std::string v_name,
                        std::string p_name);
  PointToPointPosition( const PointToPointPosition & original);
  virtual ~PointToPointPosition(){};
  // Accept default operator=; it isn't used in the implementation.

  bool check_names (std::string v_pt_name,
                    std::string p_pt_name);
};


/*****************************************************************************
PointToPointManager
Purpose:
  Manages the list of points on the vehicle, points on the planetary body, and
  the relative positions betwen them.
*****************************************************************************/
class PointToPointManager
{
 protected: // external references:

  const jeod::PlanetFixedPosition & B_wrt_P_in_P;
  const jeod::DynBody             * dyn_body;
  const jeod::RefFrameRot         * pfix_frame_rot_state;

  bool initialized;  /* (--)
      Indicates that the model has been initialized and the DynBody and
      RefFrameRot pointers have been populated with non-NULL values.*/
  std::list< PointToPointElement> vehicle_points; /* (--)
     A list of all the vehicle points in the sub-model.*/
  std::list< PointToPointElement> planet_points; /* (--)
     A list of all the planet points in the sub-model.*/
  std::list< PointToPointPosition> relative_positions; /* (--)
     A list of all the pairings of vehicle-based point with planet-based point
     from which a relative position vector can be calculated.*/


 public:
  void initialize( const jeod::DynBody     & dyn_body,
                   const jeod::RefFrameRot & pfix_frame_rot_state);
  void update();
  void add_vehicle_point( std::string pt_name,
                          double      pt_pos[3]);
  void add_planet_point(  std::string pt_name,
                          double      pt_pos[3]);
  double * add_relative_position( std::string v_pt_name,
                              std::string p_pt_name);
  void add_relative_position( std::string v_pt_name,
                              std::string p_pt_name,
                              double *& target);
  void remove_relative_position( std::string v_pt_name,
                                 std::string p_pt_name);
  void make_all_pairings();
  double * get_relative_position( std::string v_pt_name,
                                  std::string p_pt_name);

  size_t get_num_rel_pos_instances() {return relative_positions.size();}


  PointToPointManager( const jeod::PlanetFixedPosition & B_wrt_P_in_P_);
  virtual ~PointToPointManager(){};
 private:
  void add_point( std::string pt_name,
                  double      pt_pos[3],
                  std::list< PointToPointElement>  & element_list,
                  std::string list_type);

  // copy-constructor, operator= both declared private and unimplemented.
  PointToPointManager ( const PointToPointManager &);
  PointToPointManager & operator = ( const PointToPointManager &);
};
#endif
