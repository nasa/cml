/*******************************TRICK HEADER******************************
PURPOSE: (Provides an extension that computes the relative position
          between one or more points fixed to the ground and one or more points
          fixed to a vehicle.)

ASSUMPTIONS / LIMITATIONS:
  ((Each instance of PointToPointManager deals with 1 planetary body and 1
    jeod::DynBody.  It can handle multiple points on each body, but cannot handle
    relative positions between a vehicle and Earth and Moon, or between two
    vehicles and Earth)
   (This is intended to be used only as a submodel of extended planetary
    derived state; it has no independent active or subscription management, it
    is assumed that extended planetary derived state takes care of its
    subscriptions to its submodels.))

PROGRAMMERS:
  (((Gary Turner) (OSR) (May 2021) (Antares) (initial)))
***********************************************************************/

#include "../include/point_to_point.hh"


#include "jeod/models/utils/math/include/vector3.hh"

/*****************************************************************************
Constructor
*****************************************************************************/
PointToPointElement::PointToPointElement(
    std::string name_,
    double      position_[3])
{
  if (position_ == NULL) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid position specified.\n",
      "Specified position vector is NULL for point called ", name, ".\n"
      "Cannot construct a PointToPointElement with a NULL position.\n");
    return;
  }
  name = name_;
  jeod::Vector3::copy( position_, position);
}
/****************************************************************************/
PointToPointPosition::PointToPointPosition(
    std::string v_name_,
    std::string p_name_)
  :
  position{0,0,0},
  v_pos{0,0,0},
  p_pos{0,0,0},
  v_name(v_name_),
  p_name(p_name_)
{}
/****************************************************************************/
PointToPointManager::PointToPointManager(
    const jeod::PlanetFixedPosition & B_wrt_P_in_P_)
  :
  B_wrt_P_in_P( B_wrt_P_in_P_),
  dyn_body( NULL),
  pfix_frame_rot_state( NULL)
{}

/*****************************************************************************
Copy-constructors
*****************************************************************************/
PointToPointElement::PointToPointElement(
    const PointToPointElement & orig)
  :
  name(orig.name)
{
  jeod::Vector3::copy( orig.position, position);
}
/****************************************************************************/
PointToPointPosition::PointToPointPosition(
    const PointToPointPosition & orig)
  :
  v_name( orig.v_name),
  p_name( orig.p_name)
{
  jeod::Vector3::copy( orig.position, position);
  jeod::Vector3::copy( orig.v_pos, v_pos);
  jeod::Vector3::copy( orig.p_pos, p_pos);
}

/*****************************************************************************
check_names
Purpose:
  Verify whether this instance has the 2 specified names.  Used to find the
  instance corresponding to the specified names.
*****************************************************************************/
bool
PointToPointPosition::check_names(
    std::string v_pt_name,
    std::string p_pt_name)
{
  return ((v_pt_name == v_name) && (p_pt_name == p_name));
}

/*****************************************************************************
initialize
Purpose:
  verifies that the pfix frame and jeod::DynBody have been established.
  These are typically not available -- or not assignable to the model -- at
  construction time because they are model configurations, set in the input
  processor.
*****************************************************************************/
void
PointToPointManager::initialize(
    const jeod::DynBody     & dyn_body_,
    const jeod::RefFrameRot & pfix_frame_rot_state_)
{
  dyn_body = &dyn_body_;
  pfix_frame_rot_state = & pfix_frame_rot_state_;
  initialized = true;
}

/*******************************************************************************
PointToPointManager::update
Purpose:
  Updates all of the PointToPointPosition instances found in relative_positions
*******************************************************************************/
void
PointToPointManager::update()
{
  if (!initialized) {
    return;
  }
  // Nomenclature:
  // V: vehicle point position
  // G: ground-point position
  // P: pfix frame
  // B: vehicle body frame
  // S: vehicle structure frame
  // I|J:K  location of I with resepct to J expressed in K
  // T_{L->M} frame-transformation from L to M

  // We need V|G:P
  // V|G:P = V|S:P + S|B:P + B|P:P + P|G:P
  //       = T_{S->P} ( V|S:S - B|S:S) + B|P:P - G|P:P
  //       = T_{I->P} T_{S->I} ( V|S:S - B|S:S) + B|P:P - G|P:P

  // T_{I->P} = planet.pfix.state.rot.T_parent_this
  // T_{I->S} = subject.structure.state.rot.T_parent_this
  // V|S:S    = relative_position.v_pos
  // B|S:S    = subject.composite_properties.position
  // B|P:P    = PlanetaryDerivedState::state.trans.position
  // G|P:P    = relative_position.p_pos

  for (PointToPointPosition& rel_pos :relative_positions) {
    double scratch[3];
    jeod::Vector3::diff (rel_pos.v_pos,
                   dyn_body->mass.composite_properties.position,
                   scratch);   // V|B:S
    jeod::Vector3::transform_transpose( dyn_body->structure.state.rot.T_parent_this,
                                  scratch); // V|B:I
    jeod::Vector3::transform( pfix_frame_rot_state->T_parent_this,
                        scratch); //  V|B:P
    jeod::Vector3::incr( B_wrt_P_in_P.cart_coords,
                   scratch);  // V|P :P
    jeod::Vector3::diff( scratch,
                   rel_pos.p_pos,     // G|P:P
                   rel_pos.position); // V|G:P
  }
}


/*****************************************************************************
add_vehicle_point
Purpose:
  Adds a vehicle point to the list.  This point can be combined with any
  planet-point to create a relative-position instance)
*****************************************************************************/
void
PointToPointManager::add_vehicle_point(
    std::string pt_name,
    double pt_pos[3])
{
  add_point( pt_name, pt_pos, vehicle_points, "vehicle");
}
/*****************************************************************************
add_planet_point
Purpose:
  Adds a planet-point to the list.  This point can be combined with any
  vehicle-point to create a relative-position instance)
*****************************************************************************/
void
PointToPointManager::add_planet_point(
    std::string pt_name,
    double pt_pos[3])
{
  add_point( pt_name, pt_pos, planet_points, "planet");
}
/*****************************************************************************
add_point
Purpose:
  Private method implementing the common logic for add_planet_point and
  add_vehicle_point.
*****************************************************************************/
void
PointToPointManager::add_point(
  std::string pt_name,
  double pt_pos[3],
  std::list<PointToPointElement> & element_list,
  std::string list_type)
{
  // pt_pos is checked for NULL on the passthrough when creating the new
  // element.
  if (pt_name.empty()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid name specified.\n",
      "Specified name is empty.\n"
      "Cannot have an un-named point.\n",
      pt_name);
    return;
  }

  // Note -- here and throughout:
  // I'm using an iterator rather than "auto it: vehicle_points" to avoid
  // unnecessary use of the copy constructor.  It's not a big deal either
  // way here (copy constructor is cheap) but becomes a problem where the
  // logic needs to assign into the list element which absolutely requires
  // the use of an iterator rather than a copy.
  // For consistency, I'm using the same pattern throughout.
  for (auto it = element_list.begin();
            it != element_list.end(); ++it) {
    if ((*it).name == pt_name) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid name specified.\n",
        "Specified name (", pt_name, ") is already registered as a ", list_type, "-point.\n"
        "Cannot duplicate point names.\n");
      return;
    }
  }

  PointToPointElement new_pt( pt_name, pt_pos);
  if (!new_pt.name.empty()) {
    element_list.push_back(new_pt);
  }
}

/*****************************************************************************
add_relative_position
Purpose:
  Adds an instance to compute the relative position of the vehicle point with
  respect to the planet point for a specified pair of points.
  There are 2 implementations:
    2-arg: creates and populates a new relative-position and returns the
           address of the new instance's position vector.
    3-arg: calls the 2-arg implementation and assigns the returned address to
           the 3rd argument
*****************************************************************************/
double *
PointToPointManager::add_relative_position(
    std::string v_pt_name,
    std::string p_pt_name)
{
  // Check for a pre-existing match
  for (auto it_r = relative_positions.begin();
            it_r != relative_positions.end(); ++it_r) {
    if ((*it_r).check_names( v_pt_name, p_pt_name)) {
      CMLMessage::error(
        __FILE__,__LINE__,"Invalid name specified.\n",
        "Relative Position instance between ", v_pt_name, " and ", p_pt_name, " is already registered.\n"
        "Will not add a second instance of the same.\n");
      // Return the address to the pre-existing instance.
      return get_relative_position( v_pt_name, p_pt_name);
    }
  }

  PointToPointPosition new_rel_pos(v_pt_name, p_pt_name);
  int match = 0;
  for (auto it_v = vehicle_points.begin();
            it_v != vehicle_points.end(); ++it_v) {
    if ((*it_v).name == v_pt_name) {
      jeod::Vector3::copy( (*it_v).position, new_rel_pos.v_pos);
      match++;
      break;
    }
  }
  for (auto it_p = planet_points.begin();
            it_p != planet_points.end(); ++it_p) {
    if ((*it_p).name == p_pt_name) {
      jeod::Vector3::copy( (*it_p).position, new_rel_pos.p_pos);
      match++;
      break;
    }
  }
  if (match !=2) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid name specified.\n",
      "Could not match the specified names (", v_pt_name, ", ", p_pt_name, ") with a registered\n"
      "vehicle-point-name and planet-point-name respectively.\n"
      "Cannot compute the relative position between these two points.\n");
    return NULL;
  }

  relative_positions.push_back( new_rel_pos);
  return relative_positions.back().position;
}
/****************************************************************************/
void
PointToPointManager::add_relative_position(
    std::string v_pt_name,
    std::string p_pt_name,
    double    *& target)
{
  target = add_relative_position( v_pt_name, p_pt_name);
}


/*****************************************************************************
remove_relative_position
Purpose:
  Remove a PointToPointPosition instance that is no longer needed.
*****************************************************************************/
void
PointToPointManager::remove_relative_position(
    std::string v_pt_name,
    std::string p_pt_name)
{
  for (auto it_r = relative_positions.begin();
            it_r != relative_positions.end(); ++it_r) {
    if ((*it_r).check_names( v_pt_name, p_pt_name)) {
      relative_positions.erase(it_r);
      return;
    }
  }
  CMLMessage::warn(
    __FILE__,__LINE__,"Invalid name specified.\n",
    "Could not match the specified names (", v_pt_name, ", ", p_pt_name, ") to a registered\n"
    "instance of a PointToPointPosition.\n"
    "Nothing to be removed; relative_positions list remains unaltered.\n");
}


/*****************************************************************************
get_relative_position
Purpose:
  Returns a pointer to the position array of the PointToPointPosition instance
  matching the specified names.
*****************************************************************************/
double *
PointToPointManager::get_relative_position(
  std::string v_pt_name,
  std::string p_pt_name)
{
  for (auto it_r = relative_positions.begin();
            it_r != relative_positions.end(); ++it_r) {
    if ((*it_r).check_names( v_pt_name, p_pt_name)) {
      return (*it_r).position;
    }
  }
  CMLMessage::error(
    __FILE__,__LINE__,"Invalid names specified.\n",
    "Could not find a relative-position instance with point names ", v_pt_name, " and ", p_pt_name, ".\n"
    "No relative position available.\n");
  return NULL;
}

/*****************************************************************************
make_all_pairings
Purpose:
  Pair every element in the list of vehicle points with every element in the
  list of planet points, creating a PointToPointPosition instance between them
  and adding it to the relative_positions list.
*****************************************************************************/
void
PointToPointManager::make_all_pairings()
{
  for (auto it_v = vehicle_points.begin();
            it_v != vehicle_points.end(); ++it_v) {
    std::string v_name = (*it_v).name;
    for (auto it_p = planet_points.begin();
              it_p != planet_points.end(); ++it_p) {
      std::string p_name = (*it_p).name;
      bool match = false;
      for (auto it_r = relative_positions.begin();
                it_r != relative_positions.end(); ++it_r) {
        if ((*it_r).check_names( v_name, p_name)) {
          match = true;
          break;
          // silently move on to the next pair.
        }
      }
      if (!match) {
        // No matches in the relative_positions list.  Add this instance.
        PointToPointPosition new_rel_pos (v_name, p_name);
        jeod::Vector3::copy( (*it_v).position, new_rel_pos.v_pos);
        jeod::Vector3::copy( (*it_p).position, new_rel_pos.p_pos);
        relative_positions.push_back( new_rel_pos);
      }
    }
  }
}
