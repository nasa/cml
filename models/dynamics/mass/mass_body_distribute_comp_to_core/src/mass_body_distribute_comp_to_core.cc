/*******************************************************************************
Purpose:
  (Define the class MassBodyDistributeCompToCore.
   This class manages the application of dispersions to composite-properties
   of a compound body.
  )

Programmers:
  (((Gary Turner) (OSR) (April 2018) (Antares) (Initial version))
   ((Daniel Ghan) (OSR) (August 2020) (Antares) (Added sanity checks)))
*******************************************************************************/

#include <sstream> // ostringstream

#include "../include/mass_body_distribute_comp_to_core.hh"


/*****************************************************************************
Constructor
*****************************************************************************/
MassBodyDistributeCompToCore::MassBodyDistributeCompToCore (
    jeod::MassBody & adjustable_body_in,
    jeod::MassBody & target_body_in)
  :
  target_body( target_body_in),
  adjustable_body( adjustable_body_in),
  fail_if_mass_invalid(true),
  adjustable_struc_wrt_target_struc(),
  target_properties(),
  old_mass(0.0)
{}

/*****************************************************************************
check_configuration
Purpose:(Makes common checks and prepares the target body.)
*****************************************************************************/
bool
MassBodyDistributeCompToCore::check_configuration()
{
  // if there is no path to get from the target-body down the tree to the
  // adjustable-body, abort.
  // Note - where the two bodies are identical, is_progeny_of returns true.
  if ( !adjustable_body.is_progeny_of(target_body)) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid adjustments in Mass Tree\n",
      "The adjustable-body (", adjustable_body.name, ") is not a progeny of\n"
      "the target body (", target_body.name, ").\n"
      "Consequently, adjusting the mass properties of ", adjustable_body.name,
      "\ncannnot affect the composite-properties of ", target_body.name, ".\n"
      "Aborting adjustment.\n");
    return false;
  }

  if ( !adjustable_body.get_mass_properties_initialized() ||
       !target_body.get_mass_properties_initialized()) {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid adjustments in Mass Tree\n",
      "Bodies in the mass tree must be initialized before their properties\n"
      "can be modified.\n");
    return false;
  }

  // Apply any existing pending updates to target before starting, just to be
  // safe.  Applying updates to adjustable-body is pointless because that
  // body's properties are going to be reassigned anyway,
  target_body.update_mass_properties();
  return true;
}

/*****************************************************************************
update
Purpose:(The main executable)
*****************************************************************************/
bool
MassBodyDistributeCompToCore::update(
             const jeod::MassProperties & new_target_props)
{
  // Make configuration checks:
  if (!check_configuration()) {
    return false;
  }

  // Copy the incoming mass-properties onto the internal target_properties.
  target_properties.mass = new_target_props.mass;
  jeod::Vector3::copy( new_target_props.position,
                 target_properties.position);
  jeod::Matrix3x3::copy( new_target_props.inertia,
                   target_properties.inertia);

  // generate the properties of the adjustable body that will give the target
  // body the desired composite-properties (as passed in and just copied).

  // Start with the mass.
  old_mass = adjustable_body.core_properties.mass;

  adjustable_body.core_properties.mass += (target_properties.mass -
                                       target_body.composite_properties.mass);

  // Now get the new position.
  generate_new_position();

  // Now a touch of magic that might not make much sense.  Set the update flag
  // (this hits the adjustable-body and everything above it) and update the mass
  // from the target-body down the tree to the adjustable-body.
  // Notice that we have not worked the inertia at this point, but doing
  // this update will help with correctly working the inertia tensor. Refer to
  // the model documentation for details.
  adjustable_body.set_update_flag();
  target_body.update_mass_properties();

  // Now generate the new inertia tensor.
  generate_new_inertia();

  // Set the update-flags again and update the entire tree.
  adjustable_body.set_update_flag();
  jeod::MassBody * root_body = const_cast<jeod::MassBody *> (target_body.get_root_body());
  if (root_body != NULL) {
    root_body->update_mass_properties();
  }
  else {
    CMLMessage::error(
      __FILE__,__LINE__,"Invalid mass tree configuration.\n",
      "Failed to find the root of the tree.\n"
      "Updating the mass tree only as far as the target-body\n");
    target_body.update_mass_properties();
  }

  check_for_valid_mass_properties();
  return true;
}

/*****************************************************************************
generate_new_position
Purpose:
    (Generate the new position of the adjustable_body core CoM wrt target_body
     structure, in target_body-structure frame.
     Based on m_comp * x_comp = sum {m_i,core * x_i,core}
     recognizing that all bodies but target and adjustable cancel when
     differencing the new and old sums.
*****************************************************************************/
void
MassBodyDistributeCompToCore::generate_new_position()
     // NOTE - target_body is const in this method.
{
  // A. Start by getting the relative position and orientation of the
  //    adjustable-body structure point relative to the target-body
  //    structure-point.
  adjustable_body.structure_point.compute_state_wrt_pred(
                     target_body.structure_point,
                     adjustable_struc_wrt_target_struc);

  // B.  Now get the position of the core-properties point wrt target-body
  //     structure-point.  This will start as the old value and then be
  //     overwritten with the new value.
  double pos_adjustable_core_wrt_target_struc[3];

  // B1: original posn of adj-core wrt adj-struc in tgt-struc
  jeod::Vector3::transform_transpose(
                      adjustable_struc_wrt_target_struc.T_parent_this,
                      adjustable_body.core_properties.position,
                      pos_adjustable_core_wrt_target_struc);
  // B2: original posn of adj-core wrt tgt-struc in tgt-struc
  jeod::Vector3::incr( adjustable_struc_wrt_target_struc.position,
                 pos_adjustable_core_wrt_target_struc);

  // C: Now we have the necessary mass-points and their position vectors
  //    expressed in the same 'frame':
  //     - original adj-body core-mass
  //     - original tgt-body comp-mass
  //     - new      tgt-body comp-mass
  //    From these we can obtain the new position of the adj-body core-mass

  //    m_composite  * pos_composite  = sum {m_core * pos_core}_i
  //    m_composite' * pos_composite' = sum {m_core' * pos_core'}_i

  //    m_composite' * pos_composite' -
  //    m_composite  * pos_composite    =   {m_core' * pos_core'}_adj -
  //                                        {m_core  * pos_core}_adj
  //
  //  {pos_core'}_adj = (m_pos_comp' - m_pos_comp + m_pos_core) / m_core'

  double m_pos_adj_core_orig[3];
  jeod::Vector3::scale( pos_adjustable_core_wrt_target_struc,
                  old_mass,
                  m_pos_adj_core_orig); // intermediate step

  double m_pos_tgt_comp_orig_negative[3];
  jeod::Vector3::scale( target_body.composite_properties.position,
                  -target_body.composite_properties.mass,
                  m_pos_tgt_comp_orig_negative);

  double m_pos_tgt_comp_new[3];
  jeod::Vector3::scale( target_properties.position,
                  target_properties.mass,
                  m_pos_tgt_comp_new);

  jeod::Vector3::sum( m_pos_adj_core_orig,
                m_pos_tgt_comp_orig_negative,
                m_pos_tgt_comp_new,
                pos_adjustable_core_wrt_target_struc);

  jeod::Vector3::scale( 1/adjustable_body.core_properties.mass,
                  pos_adjustable_core_wrt_target_struc); // final valuea

  // D: Now transform that back to be a position relative to the
  // adjustable-body structure-point.

  // D1: subtract off the position of the adj-body struc-point.
  //     to give adj-struc -to- adj-core in tgt-struc
  jeod::Vector3::diff( pos_adjustable_core_wrt_target_struc,
                 adjustable_struc_wrt_target_struc.position,
                 adjustable_body.core_properties.position);
  // D2: transform back to adj-struc.
  jeod::Vector3::transform( adjustable_struc_wrt_target_struc.T_parent_this,
                      adjustable_body.core_properties.position);
}

/*****************************************************************************
generate_new_inertia
Purpose:(
   Generates the new inertia tensor of the adjsutable body
   The target-body composite-inertia will change as a result of 2 changes:
    1. changes to the intrinsic inertia tensor of the adjustable-body
    2. changes in the relative position of the adjustable-body

   The update_mass_properties method just performed (in the apply(...) method)
   has already factored in the effect of moving the adjustment-body CoM, which
   is the difficult part.
   Now just need to add in the necessary delta to the intrinsic inertia to
   get the composite-properties come out correct.
*****************************************************************************/
void
MassBodyDistributeCompToCore::generate_new_inertia()
     // NOTE - target_body is const in this method.
{
  // A: First find out by how much the target inertia needs to change
  double delta_inertia[3][3];
  jeod::Matrix3x3::subtract( target_properties.inertia,
                       target_body.composite_properties.inertia,
                       delta_inertia);

  // B: Apply frame transformations- the delta-inertia is in the target-body
  // coordinate system, whereas the inertia of interest is in the
  // adjustment-body coordinates.  So need to transform the
  // delta-inertia so that it can be added to the adjustment-body
  // Non-trivial calculation, it's worth checking that it is really necessary.
  if (&target_body != &adjustable_body) {
    // The body-to-body transformation can be extracted from the
    // structure-to-structure and both structure-to-body transformations.
    double T_target_body_to_adjustable_body[3][3];
    double scratch_mx[3][3];
    // T_{tgt_body -> adj_bdy} = (T_{adj_str->adj_bdy}  *
    //                            T_{tgt_str->adj_str}) *
    //                            T_{tgt_bdy->tgt_str}
    jeod::Matrix3x3::product( adjustable_body.core_properties.T_parent_this,
                        adjustable_struc_wrt_target_struc.T_parent_this,
                        scratch_mx);
    jeod::Matrix3x3::product_right_transpose(
                        scratch_mx,
                        target_body.core_properties.T_parent_this,
                        T_target_body_to_adjustable_body);
    // NOTE - passing identical arguments for the 2nd and 3rd arguments
    //        is safe in this application.  The method has an
    //        internally-generated local variable to store the
    //        intermediate values.
    jeod::Matrix3x3::transform_matrix( T_target_body_to_adjustable_body,
                                 delta_inertia,
                                 delta_inertia);
  }
  // Now add in the difference.
  jeod::Matrix3x3::incr(  delta_inertia,
                    adjustable_body.core_properties.inertia);
}

/*****************************************************************************
check_for_valid_mass_properties
Purpose:(Sanity checks on the new mass properties)
*****************************************************************************/
void
MassBodyDistributeCompToCore::check_for_valid_mass_properties()
{
  // In order for the new mass properties to be valid (physically possible):
  // 1. The mass must be positive.
  // 2. The principal moments of inertia must be positive.
  // 3. The major moment of inertia must be less than the sum of the minor and
  //    intermediate moments of inertia.
  // The principal moments of inertia are the eigenvalues of the inertia tensor.
  // TODO Ghan August 2020 Solve for the eigenvalues of the inertia tensor
      // Finding the eigenvalues of a 3x3 matrix requires solving a cubic
      // equation, which is not trivial. This capability should be added to
      // MathUtils, as it may be useful in other CML models as well. In the
      // meantime, I've substituted the following sanity checks:
      // 1. The diagonals of the inertia tensor (the moments of inertia about
      //    the axes of the structural coordinate system) must be positive.
      // 2. The greatest of the diagonals of the inertia tensor must be less
      //    than the sum of the other two.
      // 3. The determinant of the inertia tensor (which is the product of the
      //    eigenvalues) must be positive.
      // These checks are not sufficient to guarantee that the principal moments
      // of inertia are valid - for example, the matrix
      // [2500, 3062, 3062]                                                       
      // [3062, 3749, 3750]                                                       
      // [3062, 3750, 3749]
      // would satisfy the current checks but has two negative eigenvalues.

  // Create a short name for the inertia tensor to make this code more readable
  double (&I)[3][3](adjustable_body.core_properties.inertia);
  // If any error message is generated, this will be the beginning of it
  std::ostringstream err_msg;
  err_msg << "It is physically impossible to achieve specified composite mass\n"
    << "properties of " << target_body.name << " by changing the core mass\n"
    << "properties of " << adjustable_body.name << ":\n";
  bool mass_invalid = false;
  if (adjustable_body.core_properties.mass <= 0.0) {
     err_msg << "* The mass would have a non-positive value of "
       << adjustable_body.core_properties.mass << "\n";
     mass_invalid = true;
  }
  if (I[0][0] <= 0 || I[1][1] <= 0 || I[2][2] <= 0 ||
      I[0][0] >= I[1][1] + I[2][2] || I[1][1] >= I[0][0] + I[2][2]
                                   || I[2][2] >= I[0][0] + I[1][1]) {
    err_msg << "* One of the moments of inertia is non-positive or not less\n"
      << "  than the sum of the other two. Moments of inertia: " << I[0][0]
      << ", " << I[1][1] << ", " << I[2][2] << "\n";
    mass_invalid = true;
  }
  if (I[0][0]*I[1][1]*I[2][2] + I[0][1]*I[1][2]*I[2][0] +
      I[0][2]*I[1][0]*I[2][1] - I[0][0]*I[1][2]*I[2][1] -
      I[0][1]*I[1][0]*I[2][2] - I[0][2]*I[1][1]*I[2][0] <= 0) {
    // Determinant is non-positive
    err_msg << "* At least one of the principal moments of inertia is "
      << "non-positive\n";
    mass_invalid = true;
  }
  if (mass_invalid) {
    if (fail_if_mass_invalid) {
      CMLMessage::fail(__FILE__, __LINE__,
        "Invalid mass properties resulting from MassBodyDistributeCompToCore\n",
         err_msg.str(), "Terminating simulation.\n");
    }
    else {
      CMLMessage::error(__FILE__, __LINE__,
        "Invalid mass properties resulting from MassBodyDistributeCompToCore\n",
         err_msg.str(), "Because the fail_if_mass_invalid flag has been set to false, the\n"
        "simulation will continue with impossible mass properties for ", adjustable_body.name, ".\n");
    }
  }
}
