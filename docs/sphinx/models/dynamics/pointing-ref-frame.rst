Pointing Reference Frame
++++++++++++++++++++++++

.. list-table:: Revision History
   :widths: 15 30 30 50
   :header-rows: 1

   * - Version
     - Date
     - Author
     - Purpose
   * - 1
     - August 2019
     - Gary Turner
     - Initial version
   * - 2
     - September 2025
     - Hirad Mirhashemi/Alexandre Masset
     - Refactored into new template, added mathematical formulation, and data verification procedures.
   * - 3
     - August 2026
     - Nino Tarantino
     - Converted to reStructuredText and updated coverage 

.. contents:: Table of Contents
   :local:
   :class: this-will-duplicate-information-and-it-is-still-useful-here

________________________________________________________

Introduction
============

The Pointing Reference Frame model provides an additional reference frame defined by two
pre-existing frames and the following rules:

1. The origin of the new frame is at the origin of one of the two pre-existing frames, the
   *Originating-Frame.*
2. The x-axis of the new frame is aligned with the vector from the origin of the originating-frame
   to the origin of the other pre-existing frame, the *Target-Frame*.
3. The z-axis of the new frame is aligned with the angular momentum vector resulting from the
   relative linear motion of the target frame with respect to the originating frame.
4. The y-axis of the new frame completes the right-handed coordinate system.

A common application of this type of implementation is in creating a synodic frame, a frame that is
always oriented between two bodies as they move around one another. An example of such a frame is
the Earth-Moon Rotating Reference Frame.

This model is does not inherit from the ``jeod::RefFrame`` class. However, it still largely relies on this
class as the Pointing Reference Frame built is a ``jeod::RefFrame`` object. It is advised to be familiar
with this class before using this model.

________________________________________________________

Nomenclature and Concepts
=========================

In this documentation, the Pointing Reference Frame uses the terms *Originating-Frame* and
*Target-Frame*, which are defined as follows:

-  Originating-frame: The reference frame at the origin of the Pointing Reference Frame.
-  Target-frame: The reference frame whose position relative to the originating-frame determines the
   Pointing Reference Frame's orientation.

These definitions define the Pointing Reference Frame as a reference frame that is centered at the
*Originating-Frame* and pointing from there to the *Target-Frame.*

________________________________________________________

Requirements
============

1. The model shall provide a RefFrame object that defines a Pointing Reference Frame between two
   frames: an originating frame and a target frame.

   a. The x-axis shall be defined along the position vector from the originating frame to the target
      frame.
   b. The z-axis shall be defined along the vector resulting from the cross product of the pointing
      reference frame x-axis and the relative velocity between the originating frame and the target
      frame.
   c. The y-axis shall complete this set to define a right-hand coordinate frame.
   d. These vectors must be unit vectors when defined.

2. The model shall handle limit cases when the relative position and the relative velocity are
   aligned.

3. The model shall provide optional use of Ephemerides in the construction of the pointing reference
   frame, allowing a user to use external existing reference frames for the originating frame and
   the target frame.

   a. The model shall guarantee that the Ephemerides tree is updated prior to the pointing reference
      frame.

________________________________________________________

Model Specifications
====================

Architectural Considerations
----------------------------

The model consists of the primary *PointingRefFrame* class and the specialized extension of it,
*EphemBasedPointingRefFrame*. The *PointingRefFrame* class includes access to a reference frame
instance as a class member, instead of directly inheriting from *RefFrame* to allow inheritance from
:cpp:class:`SubscriptionBase` for standard activation and deactivation of the model. The
*EphemBasedPointingRefFrame* class inherits from *PointingRefFrame* for scenarios where at least one
of the *Originating-Frame* or *Target-Frame* are frames managed by the *EphemeridesManager*. This
extended class provides a reference to the *EphemeridesManager* to update the ephemerides alongside
the *PointingRefFrame* behavior.

.. _model-structure:

Model Structure
---------------

.. doxygenfile:: pointing_ref_frame.hh

________________________________________________________

.. doxygenfile:: ephem_based_pointing_ref_frame.hh

________________________________________________________

Mathematical Formulation
------------------------

Mathematical Nomenclature
~~~~~~~~~~~~~~~~~~~~~~~~~

In this formulation, the following notation is used:

-  Vectors and matrices are denoted in bold (e.g. :math:`\mathbf{\hat{x}}`).
-  Subscripts are used to provide additional descriptions (e.g. :math:`\mathbf{R_{\mathit{rel}}}`
   describes a relative position vector)
-  Superscripts are used to provide the reference frame in which the vector is expressed (e.g.
   :math:`\mathbf{R_{\mathit{rel}}^{B}}` describes a relative position vector expressed in the B-frame).
-  Vectors provided in their vector form use subscripts to provide the reference frame in which they
   are expressed. For example: :math:`\begin{bmatrix}x \\y \\z \\\end{bmatrix}_{B}`
-  Superscripts on derivative operators are used to provide the reference frame of observation (e.g.
   :math:`\frac{{}^{I}d}{\mathit{dt}}{(\mathbf{R})}` denotes the inertial derivative of
   :math:`\mathbf{R}`, i.e. the time rate of change of :math:`\mathbf{R}` as observed from the
   inertial frame).

Following this notation, the context for each expression is provided below:

-  :math:`\mathbf{R_{\mathit{rel}}}` represents the position vector of the *Target-Frame* relative
   to the *Originating-Frame*, expressed in the inertial frame.
-  :math:`\mathbf{V_{\mathit{rel}}}` represents the velocity vector of the *Target-Frame* relative
   to the *Originating-Frame*, expressed in the inertial frame.
-  :math:`\mathbf{\hat{x}}` represents the first unit basis vector of the rotating frame, expressed
   in the inertial frame.
-  :math:`\mathbf{\hat{y}}` represents the second unit basis vector of the rotating frame, expressed
   in the inertial frame.
-  :math:`\mathbf{\hat{z}}` represents the third unit basis vector of the rotating frame, expressed
   in the inertial frame.
-  :math:`\mathbf{T_{\mathit{PRF}/I}}` represents the transformation matrix from the inertial frame
   to the rotating frame.
-  :math:`\mathbf{\Omega_{\mathit{PRF}/I}^{\mathit{PRF}}}` represents the angular velocity vector from the inertial
   frame to the rotating frame, expressed in the rotating frame.
-  :math:`\mathbf{R_{v}}` represents the inertial position vector of the vehicle.
-  :math:`\mathbf{R_{\mathit{PRF}}}` represents the inertial position vector of the rotating frame.
-  :math:`\mathbf{R_{v/\mathit{PRF}}}` represents the position vector of the vehicle relative to the
   rotating frame.
-  :math:`\mathbf{V_{v}}` represents the inertial velocity vector of the vehicle.
-  :math:`\mathbf{V_{\mathit{PRF}}}` represents the inertial velocity vector of the rotating frame.
-  :math:`\mathbf{V_{v/\mathit{PRF}}}` represents the velocity vector of the vehicle relative to the
   rotating frame, as seen from the rotating frame.

.. _pointing-ref-frame:

Pointing Reference Frame
~~~~~~~~~~~~~~~~~~~~~~~~

The Pointing Reference Frame is constructed using the motion of the *Target-Frame* relative to the
*Originating-Frame* to define the origin and orientation of the rotating coordinate system. The
x-axis points from the *Originating-Frame* to the *Target-Frame*, the z-axis is normal to the
orbital plane formed by the relative motion, and the y-axis completes the right-handed coordinate
system. While it is possible to express some of these vectors in a different reference frame as long
as a consistent frame is used, this model implementation specifically uses only inertial components
to define the Pointing Reference Frame axes, which leads to the following expressions:

.. math::
   :label: pointing-ref-frame-basis
   :nowrap:

   \begin{equation}
      \begin{aligned}
         \mathbf{\hat{x}} &= \frac{ \mathbf{R_{\mathit{rel}}} }{ \lvert \mathbf{R_{\mathit{rel}}} \rvert } \\
         \mathbf{\hat{z}} &= \frac{ \mathbf{R_{\mathit{rel}}} \times \mathbf{V_{\mathit{rel}}} }{ \lvert \mathbf{R_{\mathit{rel}}} \times \mathbf{V_{\mathit{rel}}} \rvert } \\
         \mathbf{\hat{y}} &= \mathbf{\hat{z}} \times \mathbf{\hat{x}}
      \end{aligned}
   \end{equation}

The orientation of the Pointing Reference Frame with respect to the inertial frame is then
represented by the transformation matrix:

.. math::
   :label: pointing-ref-frame-orientation
   :nowrap:

   \begin{equation}
      \mathbf{T_{\mathit{PRF}/I}} = \begin{bmatrix}
         \mathbf{{\hat{x}}^{T}} \\
         \mathbf{{\hat{y}}^{T}} \\
         \mathbf{{\hat{z}}^{T}} \\
      \end{bmatrix}
   \end{equation}

By definition, the Pointing Reference Frame is rotating uniformly about its z-axis. Its angular
velocity vector relative to the inertial frame is expressed in the Pointing Reference Frame as:

.. math::
   :label: pointing-ref-frame-ang-vel
   :nowrap:

   \begin{equation}
      \mathbf{\Omega}^{PRF}_{PRF/\mathbf{I}} = \begin{bmatrix}
         0 \\
         0 \\
         \omega_z
      \end{bmatrix}_{PRF}
   \end{equation}

Where:

.. math::
   :label: pointing-ref-frame-ang-vel-z
   :nowrap:

   \begin{equation}
   \omega_z = \frac{\mathbf{V_{\mathit{rel}}} \cdot \mathbf{\hat{y}}} {\lvert \mathbf{R_{\mathit{rel}}} \rvert}
   \end{equation}

.. math::

Note that the calculation of :math:`\mathbf{\Omega_{\mathit{PRF}/I}^{\mathit{PRF}}}` is independent
of the reference frame used to express :math:`\mathbf{R_{\mathit{rel}}}`, :math:`\mathbf{V_{\mathit{rel}}}`,
and :math:`\mathbf{\hat{y}}`. This is trivially shown by expressing the inner product with vector
multiplication: :math:`{{\mathbf{V_{\mathit{rel}}} \cdot \mathbf{\hat{y}}} = \mathbf{{V_{\mathit{rel}}}^{T}}}\mathbf{\hat{y}}`.
However, they must be expressed in the same reference frame.


Singularities in ``PointingRefFrame`` Construction
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Under nominal conditions, the Pointing Reference Frame is constructed as described in the :ref:`Pointing Reference Frame <pointing-ref-frame>` section,
using the relative position and velocity vectors of the *Target-Frame* with respect to the
*Originating-Frame* to define the axes. However, certain configurations can lead to numerically
undefined axes in this formulation. These configurations are handled as follows:


Zero Relative Position Vector
#############################

If the magnitude of :math:`\mathbf{R_{\mathit{rel}}}` is zero (i.e. the *Target-* and
*Originating-Frames* are coincident), the Pointing Reference Frame cannot be constructed
meaningfully. In this case, the transformation matrix defining the orientation is not updated from its
initialized state, maintaining it as the identity matrix. Additionally, the angular velocity is set
to zero. This ensures the axes are well defined by fixing it to the inertial frame as there is no
meaningful relative motion.


Aligned Relative Position and Velocity Vectors
##############################################

If the relative position and velocity vectors are aligned or nearly aligned, the cross product used
to define the z-axis becomes zero and numerically unstable, resulting in a poorly defined z-axis. To
address this, two alternative solutions are formulated:

1. **Use Last Known Y-Axis (Alternative 1):** If the cross product of the relative position and velocity vectors are
   near zero, the z-axis is computed by using the previous y-axis vector in place of the relative
   velocity vector:

   .. math::
      :label: aligned-position-velocity-alternative-1
      :nowrap:

      \begin{equation}
         \mathbf{\overrightarrow{z}} = \mathbf{R_{\mathit{rel}}} \times \mathbf{\hat{y}}_{prev}
      \end{equation}

   If the resulting vector has a non-zero magnitude, it is normalized and used as the new z-axis and
   an appropriate warning is broadcasted.

2. **Use Previous Z-Axis (Alternative 2):** If Alternative 1 fails (i.e. the previous y-axis is also
   aligned with the current relative position vector), the z-axis is reconstructed using the
   previous z-axis and the current x-axis:

   .. math::
      :label: aligned-position-velocity-alternative-2
      :nowrap:

      \begin{equation}
         \mathbf{\overrightarrow{z}} = \mathbf{\hat{x}} \times (\mathbf{\hat{z}}_{prev} \times \mathbf{\hat{x}})
      \end{equation}

   This is a *vector triple product*, which ensures that the resulting z-axis is orthogonal to the
   current x-axis (i.e. the relative position vector). This formulation cannot produce a zero
   vector, because the current x-axis is aligned with the previous y-axis (as established from
   failing Alternative 1), and the previous y- and z-axes are by nominal construction orthogonal.
   Therefore, the previous z-axis cannot be aligned with the current x-axis, ensuring that this
   cross product produces a defined z-axis.

These alternatives together are a robust fallback for defining the z-axis in all cases where the
relative position and velocity vectors are aligned.


Relative Derived State
~~~~~~~~~~~~~~~~~~~~~~

The ``RelativeDerivedState`` model belongs to JEOD and is independent of the formulation
described above. This formulation will present the relative position and velocity which is
implemented in the ``RelativeDerivedState``, but is commonly used alongside the rotating frame model
and formulation provided earlier. For more information about the `RelativeDerivedState` model,
please refer to the JEOD documentation.

Once the Pointing Reference Frame is established, the position of a vehicle relative to this
Pointing Reference Frame can be determined by translating its inertial position into the frame's
origin and applying the transformation matrix from the inertial to the Pointing Reference Frame:

.. math::
   :label: position-in-prf
   :nowrap:

   \begin{equation}
      \mathbf{R_{v/\mathit{PRF}}^{PRF}} = \mathbf{T_{\mathit{PRF}/I}} \left( \mathbf{R_{v}^{I}} - \mathbf{R_{\mathit{PRF}}^{I}} \right)
   \end{equation}

where :math:`\mathbf{R_{v}^{I}}` and :math:`\mathbf{R_{\mathit{PRF}}^{I}}` are the inertial position vectors for the
vehicle and the origin of the Pointing Reference Frame, respectively, expressed in the inertial frame. As a result,
:math:`\mathbf{R_{v/\mathit{PRF}}^{\mathit{PRF}}}` is the relative position of the vehicle with respect to the origin
of the pointing reference frame, expressed in the Pointing Reference Frame.

It is important to emphasize that relative velocity as seen from the Pointing Reference Frame, isn't
simply:

.. math:: \frac{{}^{\mathit{PRF}}d}{\mathit{dt}}{{(\mathbf{R_{v/\mathit{PRF}}})} \neq \mathbf{T_{\mathit{PRF}/I}}}\frac{{}^{I}d}{\mathit{dt}}{(\mathbf{R_{v/\mathit{PRF}}})}

Instead, the velocity of the vehicle as seen from the Pointing Reference Frame is computed using the
transport theorem to account for the rotation of the Pointing Reference Frame with respect to the
inertial. Let :math:`\mathbf{V_{v}} = \frac{{}^{I}d}{\mathit{dt}}{(\mathbf{R_{v}})}` denote the inertial
velocity of the vehicle, :math:`\mathbf{V_{\mathit{PRF}}} = \frac{{}^{I}d}{\mathit{dt}}{(\mathbf{R_{\mathit{PRF}}})}`
denote the inertial velocity of the origin of the Pointing Reference Frame, and
:math:`\mathbf{V_{v/\mathit{PRF}}} = \frac{{}^{\mathit{PRF}}d}{\mathit{dt}}{(\mathbf{R_{v/\mathit{PRF}}})}`
denote the vehicle velocity with respect to the origin of the Pointing Reference Frame as seen
from the Pointing Reference Frame. Applying transport theorem, this relative velocity of the vehicle
can be expressed as:

.. math:: \mathbf{V_{v/\mathit{PRF}}} = \frac{{}^{I}d}{\mathit{dt}}{{(\mathbf{R_{v/\mathit{PRF}}})} - {\mathbf{\Omega_{\mathit{PRF}/I}} \times \mathbf{R_{v/\mathit{PRF}}}}}

Substituting the inertial velocities of the vehicle and the Pointing Reference Frame origin, this
becomes:

.. math:: \mathbf{V_{v/\mathit{PRF}}} = {{({\mathbf{V_{v}} - \mathbf{V_{\mathit{PRF}}}})} - {\mathbf{\Omega_{\mathit{PRF}/I}} \times \mathbf{R_{v/\mathit{PRF}}}}}

Finally, expressing the terms in a common Pointing Reference Frame yields:

.. math::
   :label: velocity-in-prf
   :nowrap:

   \begin{equation}
      \mathbf{V_{v/\mathit{PRF}}^{PRF}} =
         \mathbf{T_{\mathit{PRF}/I}} ({\mathbf{V_{v}^{I}} - \mathbf{V_{\mathit{PRF}}^{I}}}) -
         \mathbf{\Omega_{\mathit{PRF}/I}^{PRF}} \times \mathbf{R_{v/\mathit{PRF}}^{PRF}}
   \end{equation}

where :math:`\mathbf{V_{v}^{I}}` and :math:`\mathbf{V_{\mathit{PRF}}^{I}}` are the inertial velocity
vectors for the vehicle and the origin of the rotating frame, respectively.

________________________________________________________

User's Guide
============

Implementation
--------------

See the :ref:`Model Structure <model-structure>` section for include paths for the :cpp:class:`PointingRefFrame`
and :cpp:class:`EphemBasedPointingRefFrame` classes.

Initialization
--------------

The non-ephemerides :cpp:class:`PointingRefFrame` has a default constructor that
takes no arguments. The ephemerides Pointing Reference Frame constructor takes a reference to the Ephemerides Manager.
This is often provided as a reference to the ``jeod::DynManager``, which is a derivative of the
Ephemerides Manager:

.. code-block:: cpp

   PointingRefFrame non_ephem_rotating_frame;
   EphemBasedPointingRefFrame ephem_rotating_frame(dyn_manager);

For both Pointing Reference Frames, set the *Originating-Frame* and *Target-Frame,* and a name for
the Pointing Reference Frame like:

.. code-block:: cpp

   non_ephem_rotating_frame.set_originating_frame(vehicleA.composite_body);
   non_ephem_rotating_frame.set_target_frame(vehicleB.composite_body);
   non_ephem_rotating_frame.pointing_frame.set_name("VehicleAB-rotating-frame");

   ephem_rotating_frame.set_originating_frame(earth.planet.inertial);
   ephem_rotating_frame.set_target_frame(sun.planet.inertial);
   ephem_rotating_frame.pointing_frame.set_name("EarthSun-rotating-frame");

For the non-ephemerides Pointing Reference Frame, you will need to add the frame to the Dynamics
Manager:

.. code-block:: cpp

   dyn_manager.add_ref_frame(non_ephem_rotating_frame.pointing_frame);

The ephemerides Pointing Reference Frame does not require this step, as the model handles it
internally with the reference to the Ephemerides Manager.

The method to schedule for initialization of the model:

.. code-block:: cpp

   P_ENV ("initialization") non_ephem_rotating_frame.initialize();

   P_DYN ("initialization") ephem_rotating_frame.initialize();

The initialization of the non-ephemerides Pointing Reference Frame can occur as early as the
reference frame initialization in ``P_ENV`` or later. However, the ephemerides pointing reference
frame should occur quite late in the initialization sequence as it should be after the Ephemeris
initialization, so ``P_DYN`` or later.

Routine Execution
-----------------

The method to schedule for updates to the model:

.. code-block:: cpp

   P_ENV (DYNAMICS, "environment") non_ephem_rotating_frame.update();

   P_DYN (DYNAMICS, "environment") ephem_rotating_frame.update();

Configuration
-------------

The model is inactive by default. To activate it, you could call the :cpp:func:`~SubscriptionBase::subscribe`
method:

.. code-block:: cpp

   non_ephem_rotating_frame.subscribe();

   ephem_rotating_frame.subscribe();


Relative Derived State
~~~~~~~~~~~~~~~~~~~~~~

One of the most common uses of this frame is for expressing the state of a vehicle. The model itself
provides only the frame, the state relative to that frame can be described using an instance of
JEOD's ``jeod::RelativeDerivedState`` model. See the JEOD documentation for information on configuring
this relative state.

Of particular relevance to this model, the configuration of a ``jeod::RelativeDerivedState`` requires
specification of the *Subject-Frame* and the *Target-Frame*. Typically, the *Subject-Frame* is the
vehicle frame of interest and the *Target-Frame* is the Pointing Reference Frame defined by this
model. To set the *Target-Frame*, it is necessary to know the name of the Pointing Reference Frame,
which is set in the initialization steps.

For example, to get the relative state of the vehicle to the Earth-Sun Pointing Reference Frame:

.. code-block:: cpp

   jeod::RelativeDerivedState veh_wrt_prf;

   veh_wrt_prf.subject_frame_name = "test_vehicle.composite_body";
   veh_wrt_prf.target_frame_name = "EarthSun-rotating-frame";


Logging
~~~~~~~

Typically, the desired output is the state of some vehicle with respect to the pointing frame, for
example:

.. code-block:: cpp

   relative_derived_state_instance.rel_state.trans.position

The pointing frame itself also has a state. The position and velocity of the frame are not of
interest because they are locked to the *Originating-Frame*, so both position and velocity are zero.
However, the orientation and angular rate are occasionally useful. These are represented as follows:

-  Transformation matrix from the parent planet-inertial state to the pointing frame (3x3 matrix).

   .. code-block:: cpp

      rotating_frame_instance.pointing_frame.state.rot.T_parent_this

-  Left-handed transformation quaternion describing the transformation from the parent
   *Originating-Frame* state to the pointing frame, expressed as a scalar and a 3-element vector.

   .. code-block:: cpp

      rotating_frame_instance.pointing_frame.state.rot.Q_parent_this.scalar
      rotating_frame_instance.pointing_frame.state.rot.Q_parent_this.vector

-  Angular rate of the rotating frame relative to the parent *Originating-Frame* state, expressed in the
   rotating frame as a 3-element vector. By definition of the rotating frame, the x- and
   y-components are identically zero.

   .. code-block:: cpp

      rotating_frame_instance.pointing_frame.state.rot.ang_vel_this

________________________________________________________


Verification
============

Code Coverage
-------------

.. code-block:: text

   ------------------------------------------------------------------------------
                           GCC Code Coverage Report
   Directory: .
   ------------------------------------------------------------------------------
   File                                       Lines     Exec  Cover   Missing
   ------------------------------------------------------------------------------
   models/dynamics/state_descriptors/pointing_ref_frame/include/ephem_based_pointing_ref_frame.hh
                                                 14       14   100%
   models/dynamics/state_descriptors/pointing_ref_frame/src/pointing_ref_frame.cc
                                                 87       87   100%
   ------------------------------------------------------------------------------
   TOTAL                                        101      101   100%
   ------------------------------------------------------------------------------

Exceptions
----------

N/A

Simulation Configurations
-------------------------

SIM_verif
~~~~~~~~~

This verification simulation tests the geometry and vector operations used in defining the pointing
reference frame using two arbitrary reference frames A and B corresponding to the
*Originating-Frame* and the *Target-Frame*, respectively. Additionally, a subject vehicle is used to
verify the relative velocity of the body to the Pointing Reference Frame, as seen from the pointing
reference frame.

#. 

   #. 

      #. 

         #. .. rubric:: 
               :name: section-2

         #. .. rubric:: RUN_01_Geometric
               :name: run_01_geometric

This test verifies that the Pointing Reference Frame is properly constructed with the correct
orientation and angular velocity throughout a variety of configurations, and that the relative
position and velocity of the subject vehicle with respect to the Pointing Reference Frame is
correct.

The default setup for these unit tests includes these states in the inertial axes (+X, +Y, +Z):

-  originating-frame is at rest at the origin:
-  target-frame is located atand moving in
-  vehicle is located atand moving in

All references to the vehicle relative position and velocity are with respect to the origin of the
Pointing Reference Frame and are expressed in the Pointing Reference Frame axes. The relative
velocity is also observed from the perspective of the Pointing Reference Frame.

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Default Configuration
                  :name: default-configuration

Setup: All input states are in the default configuration.

**Expected Results:** Pointing frame aligns with +X (*Target-Frame* direction), +Y (*Target-Frame*
velocity), +Z (angular momentum direction). Vehicle lies on pointing frame x-axis and its relative
velocity opposes the *Target-Frame* motion.

Results:

+----------+-------------+-------------+-------------+-------------+-------------+-------------+
| Time (s) | .. math:    | .. math:    | .. math::   | ..          | ..          | ..          |
|          | : \mathbf{R | : \mathbf{V |  \omega_{z} | math:: \mat | math:: \mat | math:: \mat |
|          | _{v/\mathit | _{v/\mathit |             | hbf{{\wideh | hbf{{\wideh | hbf{{\wideh |
|          | {PRF}}^{\ma | {PRF}}^{\ma |             | at{x}}^{T}} | at{y}}^{T}} | at{z}}^{T}} |
|          | thit{PRF}}} | thit{PRF}}} |             |             |             |             |
+----------+-------------+-------------+-------------+-------------+-------------+-------------+
| 0        | [10,0,0]    | [0,-5,0]    | 0.5         | [1,0,0]     | [0,1,0]     | [0,0,1]     |
+----------+-------------+-------------+-------------+-------------+-------------+-------------+

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: 
                  :name: section-3

            #. .. rubric::  Offset Positions in 3D
                  :name: offset-positions-in-3d

**Setup:** *Originating-*, *Target-Frames*, and vehicle positions are all offset by [5,5,5] to
maintain same relative position and motion.

**Expected Results:** Same pointing frame and vehicle relative state as default configuration.

Results:

= ======== ======== === ======= ======= =======
1 [10,0,0] [0,-5,0] 0.5 [1,0,0] [0,1,0] [0,0,1]
= ======== ======== === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Vehicle Diagonally Located in 3D
                  :name: vehicle-diagonally-located-in-3d

**Setup:** Vehicle position is diagonally offset in the YZ-plane to be located at [10,-10,10].

**Expected Results:** Same pointing frame as default configuration. Vehicle relative position is the
same as the vehicle inertial position, and it moves in the pointing frame x- and y-axes.

Results:

= =========== ========= === ======= ======= =======
2 [10,-10,10] [-5,-5,0] 0.5 [1,0,0] [0,1,0] [0,0,1]
= =========== ========= === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Vehicle Moving in +X
                  :name: vehicle-moving-in-x

**Setup:** Vehicle now has velocity of [10,0,0] (along +X), while the *Target-Frame* still moves in
+Y with the same velocity [0,5,0].

**Expected Results:** Same pointing frame and vehicle relative position as default configuration.
Vehicle relative velocity now reflects the vehicle inertial motion along the pointing frame x-axis,
in addition to the previous opposing *Target-Frame* motion in the pointing frame y-axis.

Results:

= ======== ========= === ======= ======= =======
3 [10,0,0] [10,-5,0] 0.5 [1,0,0] [0,1,0] [0,0,1]
= ======== ========= === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Vehicle Moving in +X, -Y, and +Z
                  :name: vehicle-moving-in-x--y-and-z

**Setup:** Vehicle now has velocity of [10,-10,10], while the *Target-Frame* still moves in +Y with
the same velocity [0,5,0].

**Expected Results:** Same pointing frame and vehicle relative position as default configuration.
Vehicle relative velocity now reflects the vehicle inertial motion along the pointing frame x-, y-,
and z-axes, in addition to the previous opposing *Target-Frame* motion in the pointing frame y-axis.

Results:

= ======== =========== === ======= ======= =======
4 [10,0,0] [10,-15,10] 0.5 [1,0,0] [0,1,0] [0,0,1]
= ======== =========== === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target Moving in -Y
                  :name: target-moving-in--y

**Setup:** *Target-Frame* now moves in opposite direction (along -Y) with velocity [0,-5,0].

**Expected Results:** Same pointing frame x-axis and vehicle relative state as default
configuration. Pointing frame y- and z-axes are in the opposite direction.

Results:

= ======== ======== === ======= ======== ========
5 [10,0,0] [0,-5,0] 0.5 [1,0,0] [0,-1,0] [0,0,-1]
= ======== ======== === ======= ======== ========

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target Moving in +Z
                  :name: target-moving-in-z

**Setup:** *Target-Frame* now moves in +Z with velocity [0,0,5] instead of +Y.

**Expected Results:** Same pointing frame x-axis and vehicle relative state as default
configuration. With the *Target-Frame* moving in +Z, the pointing frame z-axis (angular momentum
vector) shifts to -Y, aligning the pointing frame y-axis with +Z to maintain a right-handed frame.

Results:

= ======== ======== === ======= ======= ========
6 [10,0,0] [0,-5,0] 0.5 [1,0,0] [0,0,1] [0,-1,0]
= ======== ======== === ======= ======= ========

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target in +Y, Moving in +X
                  :name: target-in-y-moving-in-x

**Setup:** *Target-Frame* lies along +Y at position [0,10,0] and moves in +X with velocity [5,0,0].

**Expected Results:** Pointing frame x-axis aligns with +Y (*Target-Frame* direction), y-axis with
+X (*Target-Frame* velocity), and z-axis with -Z (angular momentum direction). Vehicle relative
position lies on pointing frame y-axis and it moves in the direction of the pointing frame x-axis.

Results:

= ======== ======= === ======= ======= ========
7 [0,10,0] [5,0,0] 0.5 [0,1,0] [1,0,0] [0,0,-1]
= ======== ======= === ======= ======= ========

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target in +Y, Moving in +Z
                  :name: target-in-y-moving-in-z

**Setup:** *Target-Frame* lies along +Y at position [0,10,0] and moves in +Z with velocity [0,0,5].

**Expected Results:** Pointing frame x-axis aligns with +Y (*Target-Frame* direction), y-axis with
+Z (*Target-Frame* velocity), and z-axis with +X (angular momentum direction). Vehicle relative
position lies on pointing frame z-axis with zero velocity.

Results:

= ======== ======= === ======= ======= =======
8 [0,0,10] [0,0,0] 0.5 [0,1,0] [0,0,1] [1,0,0]
= ======== ======= === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Target in +Z, Moving in +X
                  :name: target-in-z-moving-in-x

**Setup:** *Target-Frame* lies along +Z at position [0,0,10] and moves in +X with velocity [5,0,0].

**Expected Results:** Pointing frame x-axis aligns with +Z (*Target-Frame* direction), y-axis with
+X (*Target-Frame* velocity), and z-axis with +Y (angular momentum direction). Vehicle lies on
pointing frame y-axis and it moves in the direction of the pointing frame x-axis.

Results:

= ======== ======= === ======= ======= =======
9 [0,10,0] [5,0,0] 0.5 [0,0,1] [1,0,0] [0,1,0]
= ======== ======= === ======= ======= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target in +Z, Moving in +Y
                  :name: target-in-z-moving-in-y

**Setup:** *Target-Frame* lies along +Z at position [0,0,10] and moves in +Y with velocity [0,5,0].

**Expected Results:** Pointing frame x-axis aligns with +Z (*Target-Frame* direction), y-axis with
+Y (*Target-Frame* velocity), and z-axis with -X (angular momentum direction). Vehicle lies on
pointing frame opposite z-axis with zero velocity.

Results:

== ========= ======= === ======= ======= ============
10 [0,0,-10] [0,0,0] 0.5 [0,0,1] [0,1,0] [**-1,0,0]**
== ========= ======= === ======= ======= ============

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target Moving Diagonally in XY
                  :name: target-moving-diagonally-in-xy

**Setup:** *Target-Frame* and vehicle located at position [10,10,0], and the *Target-Frame* moves
diagonally with velocity [-5,5,0].

**Expected Results:** Pointing frame x-axis aligns with the diagonal *Target-Frame* position vector
in XY-plane, y-axis with diagonal *Target-Frame* velocity in XY-plane, and z-axis remains aligned
with +Z (angular momentum direction). Vehicle lies on pointing frame x-axis and it moves in the
direction opposite of the pointing-frame y-axis.

Results:

== =========== =========== === ================ ================= =======
11 [14.14,0,0] [0,-7.07,0] 0.5 [0.707, 0.707,0] [-0.707, 0.707,0] [0,0,1]
== =========== =========== === ================ ================= =======

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Target and Vehicle in 3D
                  :name: target-and-vehicle-in-3d

**Setup:** *Target-Frame* and vehicle located at position [5,5,5], and the *Target-Frame* moves in
+Z with velocity [0,0,5].

**Expected Results:** Pointing frame x-axis points diagonally in 3D (*Target-Frame* direction). The
*Target-Frame* velocity in +Z produces an angular momentum vector in the XY-plane, pointing the
pointing frame z-axis diagonal in-plane. The y-axis completes the right-handed frame and will be
diagonal in 3D. Vehicle lies on the pointing frame x-axis and it moves in the direction opposite of
the pointing frame y-axis. The angular velocity about the pointing frame z-axis should be lower than
the other cases, because the *Target-Frame* velocity is less orthogonal to the position vector.

Results:

== ========== ============ ===== =================== ===================== ================
12 [8.66,0,0] [0,-4.082,0] 0.471 [0.577,0.577,0.577] [-0.408,-0.408,0.816] [0.707,-0.707,0]
== ========== ============ ===== =================== ===================== ================

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Origin Moving in -Y
                  :name: origin-moving-in--y

**Setup:** *Originating-Frame* now moves in -Y with velocity [0,-5,0], the opposite of the previous
*Target-Frame* motion.

**Expected Results:** Same pointing frame axes and vehicle relative state as default configuration.
The angular velocity around the pointing frame z-axis will double compared to the default
configuration results.

Results:

== ======== ======== === ======= ======= =======
13 [10,0,0] [0,-5,0] 1.0 [1,0,0] [0,1,0] [0,0,1]
== ======== ======== === ======= ======= =======

|image1|\ |image2|\ |image3|

|image4|\ |image5|\ |image6|

#. 

   #. 

      #. 

         #. .. rubric:: 
               :name: section-4

         #. .. rubric:: ERROR_bad_frames
               :name: error_bad_frames

This test verifies the errors that result from incorrect configurations and warnings from cases
where the Pointing Reference Frame axes aren't well defined and need to be alternatively handled
using the methodology in Section 3.2.2.a.

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Configuration Frames are NULL
                  :name: configuration-frames-are-null

If the *Originating-Frame* or the *Target-Frame* are set to a NULL value during intialization, the
model will broadcast a respective error.

.. image:: Pictures/1000000100000394000001AD99C6EFC3C1CEF683.png
   :width: 6.9252in
   :height: 3.2429in

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Changing Configuration after Model Activation
                  :name: changing-configuration-after-model-activation

If the Pointing Reference Frame model is already activated through its *subscribe()*, any attempt to
change the *Originating-Frame* or *Target-Frame* will result in the model broadcasting a respective
error.

.. image:: Pictures/100000010000039A00000194C1D28CF981755DC2.png
   :width: 6.9252in
   :height: 3.0339in

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: 
                  :name: section-5

            #. .. rubric:: Relative Position Vector is Zero (Proximity Warning)
                  :name: relative-position-vector-is-zero-proximity-warning

When the Pointing Reference Frame encounters a situation where the *Originating-Frame* and
*Target-Frame* are very close to each other, the relative position vector between the frames will
result in a zero vector, producing an undefined x-axis. The Pointing Reference Frame will result to
retaining its orientation, and making the angular velocity zero.

**Setup:** At t=0, a nominal configuration is defined, but at t=1 both the *Originating-Frame* and
*Target-Frame* are positioned at [0,0,0].

**Expected Results:** At t=1, the orientation from t=0 is retained and the frame has an angular
velocity of zero.

Results:

+----------+-------------+-------------+-------------+-------------+-------------+-------------+
| Time (s) | .. math:    | .. math:    | .. math::   | ..          | ..          | ..          |
|          | : \mathbf{R | : \mathbf{V |  \omega_{z} | math:: \mat | math:: \mat | math:: \mat |
|          | _{v/\mathit | _{v/\mathit |             | hbf{{\wideh | hbf{{\wideh | hbf{{\wideh |
|          | {PRF}}^{\ma | {PRF}}^{\ma |             | at{x}}^{T}} | at{y}}^{T}} | at{z}}^{T}} |
|          | thit{PRF}}} | thit{PRF}}} |             |             |             |             |
+----------+-------------+-------------+-------------+-------------+-------------+-------------+
| 0        | [3,2,-1]    | [8,2,-4]    | 1           | [0,0,1]     | [0,1,0]     | [-1,0,0]    |
+----------+-------------+-------------+-------------+-------------+-------------+-------------+
| 1        | [3,2,-1]    | [6,5,-4]    | 0           | [0,0,1]     | [0,1,0]     | [-1,0,0]    |
+----------+-------------+-------------+-------------+-------------+-------------+-------------+

#. 

   #. 

      #. 

         #. 

            #. .. rubric:: Relative Position and Velocity Vectors are Aligned (Alternative 1)
                  :name: relative-position-and-velocity-vectors-are-aligned-alternative-1

When the Pointing Reference Frame encounters a situation where the relative position and velocity
vectors between the *Originating-Frame* and *Target-Frame* are very close to being aligned, the
cross product of these vectors will result in a zero vector, producing an undefined z-axis. In the
case where the relative position vector is not aligned with the previous y-axis, the cross product
of these vectors produces the new z-axis. Ultimately, this results in the y-axis being retained from
the previous iteration.

**Setup:** At t=2, a nominal configuration is defined, but at t=3 the *Originating-Frame* and
*Target-Frame* are positioned at [0,0,0] and [0,0,-1], respectively, and are moving at velocities
[0,0,0] and [0,0,1], respectively.

**Expected Results:** At t=3, the relative position and velocity vectors are aligned, causing the
z-axis to be computed using the relative position vector from t=3 and y-axis from t=2, thus
preserving the previous y-axis orientation from t=2.

Results:

= ======== ======== = ======== ======= ========
2 [0,0,-1] [6,5,-4] 1 [0,0,1]  [0,1,0] [-1,0,0]
3 [0,0,1]  [-6,5,4] 0 [0,0,-1] [0,1,0] [1,0,0]
= ======== ======== = ======== ======= ========

#. 

   #. 

      #. 

         #. 

            #. .. rubric::  Relative Position and Velocity Vectors are Aligned (Alternative 2)
                  :name: relative-position-and-velocity-vectors-are-aligned-alternative-2

Similar to the previous test, when the relative position and velocity vectors are very close to
being aligned, the cross product of these vectors will produce an undefined z-axis. In the
off-chance where the relative position vector is also aligned with the previous y-axis, the
formulation from alternative 1 produces another undefined z-axis. In this case, the z-axis is
defined as the result of a triple cross product of the x-axis and the previous z-axis. Ultimately,
this results in the z-axis being retained from the previous iteration.

**Setup:** At t=3, the Pointing Reference Frame axes is fully defined using alternative 1, but at
t=4 the *Originating-Frame* and *Target-Frame* are positioned at [0,0,0] and [0,1,0], respectively,
and are moving at velocities [0,0,0] and [0,1,0], respectively. The relative position and velocity
vectors at t=4 are different from those at t=3.

**Expected Results:** At t=4, the relative position is aligned with the relative velocity vector and
the previous y-axis, causing the z-axis to be computed as the result of a triple cross product of
the x-axis from t=4 and z-axis from t=3, thus preserving the previous z-axis orientation from t=3.

Results:

= ======= ======== = ======== ======= =======
3 [0,0,1] [-6,5,4] 0 [0,0,-1] [0,1,0] [1,0,0]
4 [0,0,1] [5,6,4]  0 [0,1,0]  [0,0,1] [1,0,0]
= ======= ======== = ======== ======= =======

#. 

   #. 

      #. 

         #. .. rubric:: FAIL_unassigned_frames
               :name: fail_unassigned_frames

This test verifies that when the model is initialized without specifying the *Target-Frame* and/or
*Originating-Frame,* an error is broadcast, the model is stopped and returns the boolean *False.*

.. image:: Pictures/10000001000003960000017E0B1EF006C4362DBE.png
   :width: 6.9252in
   :height: 2.8811in

#. 

   #. 

      #. .. rubric:: SIM_Ephem
            :name: sim_ephem

This verification simulation tests the functionality of the *EphemBasedPointingRefFrame* class and
is limited to the very small differences between this class and its base class, *PointingRefFrame.*
It illustrates the potential pitfalls of using a *PointingRefFrame* instance rather than an
*EphemBasedPointingRefFrame* instance when either the *Target-Frame* and/or *Originating-Frame* are
managed by the Ephemerides Manager. See *FAIL_noephem_only* and *FAIL_noephem_with_subscriptions*
for easy mistakes to avoid.

All the runs in this simulation define the Pointing Reference Frames to track the Earth and Sun
relative positions and motion, with an individual subject vehicle fixed on the Earth-Sun vector to
enable verification of the relative state of the vehicle with respect to the pointing reference
frame.

Since the *PointingRefFrame* was already geometrically verified in *SIM_verif*, the purpose of
logging the following data is to determine which frame, *EphemBasedPointingRefFrame* or
*PointingRefFrame* is actively driving the relative state updates, and whether the inertial
positions of the Earth and Sun are being updated via the EphemeridesManager.

+---------------+---------------+---------------+---------------+---------------+---------------+
| Time (s)      | ..            | .. math:: \ma | EphemBasedPoi | Poi           |               |
|               |  math:: \math | thbf{R_{\math | ntingRefFrame | ntingRefFrame |               |
|               | bf{R_{\mathit | it{Sun}}^{I}} |               | (Non-Ephem)   |               |
|               | {Earth}}^{I}} |               |               |               |               |
+---------------+---------------+---------------+---------------+---------------+---------------+
| .. math:: \ma | .. math:: \ma | .. math       | .. math:: \ma | .. math:: \ma | .. math       |
| thbf{R_{v/\ma | thbf{V_{v/\ma | :: \omega_{z} | thbf{R_{v/\ma | thbf{V_{v/\ma | :: \omega_{z} |
| thit{PRF}}^{\ | thit{PRF}}^{\ |               | thit{PRF}}^{\ | thit{PRF}}^{\ |               |
| mathit{PRF}}} | mathit{PRF}}} |               | mathit{PRF}}} | mathit{PRF}}} |               |
+---------------+---------------+---------------+---------------+---------------+---------------+

#. 

   #. 

      #. 

         #. .. rubric:: 
               :name: section-6

         #. .. rubric:: RUN_01_ephem_only
               :name: run_01_ephem_only

This test demonstrates the recommended implementation for a Pointing Reference Frame that depends on
frames managed by the Ephemerides Manager.

Setup: Run only the *EphemBasedPointingRefFrame* and its associated relative state.

Expected Results: Only the *EphemBasedPointingRefFrame* instance and its relative vehicle state are
updated, not the *PointingRefFrame* instance. The inertial positions of the Earth and Sun are also
updated through the *EphemBasedPointingRefFrame* connection to the Ephemerides Manager.

Results:

|image7|\ |image8|

|image9|\ |image10|\ |image11|

|image12|\ |image13|\ |image14|

#. 

   #. 

      #. 

         #. .. rubric:: RUN_02_both
               :name: run_02_both

This test demontrates that since *EphemBasedPointingRefFrame* can add its Ephemeris-based frames to
the Ephemeris tree through the Ephemerides Manager, *PointingRefFrame* can indirectly track those
same frames despite not having a connection to the Ephemerides Manager. The effect of this hidden
dependency becomes evident in the *FAIL_noephem_only* test, where removing the
*EphemBasedPointingRefFrame* causes the base *PointingRefFrame* to lose access to the
Ephemeris-based frames.

Setup: Runs both the *EphemBasedPointingRefFrame,* the base *PointingRefFrame*, and their associated
relative states.

Expected Results: Both the *EphemBasedPointingRefFrame* and the *PointingRefFrame* instances are
updated with their associated relative vehicle states. The inertial positions of the Earth and Sun
are also updated through the *EphemBasedPointingRefFrame* connection to the Ephemerides Manager.

Results:

|image15|\ |image16|

|image17|\ |image18|\ |image19|

|image20|\ |image21|\ |image22|

#. 

   #. 

      #. 

         #. .. rubric:: 
               :name: section-7

         #. .. rubric:: RUN_03_ephem_only_resubscribe
               :name: run_03_ephem_only_resubscribe

This test is similar to *RUN_01_ephem_only*, in that only the *EphemBasedPointingRefFrame* and its
associated relative-state are computed. However, this test highlights the expected behavior when the
model is initially deactivated and later reactivated successfully, in constrast with
*FAIL_noephem_with_subscriptions*, where reactivation fails. It serves to verify that the
*EphemBasedPointingRefFrame* can correctly resume updating relative states and ephemeris-driven
positions after being re-subscribed mid-simulation.

Setup: Run only the *EphemBasedPointingRefFrame* and its associated relative state. Model is
initially deactivated and later reactivated during the simulation at t=300000.

Expected Results: The *EphemBasedPointingRefFrame* instance and its relative vehicle state are
initialized at t=0 and not updated anymore until after t=300000. Similarly, the Sun inertial
reference frame state won't be updated until that point as it is deactivated through
*EphemBasedPointingRefFrame's target-frame*. However, the Earth inertial reference frame state
continues updating as it is used as the integration frame for the subject vehicle.

Results:

|image23|\ |image24|\ |image25|

|image26|\ |image27|\ |image28|

|image29|\ |image30|\ |image31|

#. 

   #. 

      #. 

         #. .. rubric:: FAIL_noephem_only
               :name: fail_noephem_only

This test runs only the base *PointingRefFrame* and its associated relative state while still using
Ephemeris-based frames like the Earth inertial and Sun inertial as the *Originating-Frame* and
*Target-Frame*. The *PointingRefFrame* doesn't have access to the Ephemerides Manager unlike
*EphemBasedPointingRefFrame*, which causes the test to fail as the Ephemeris reference frame tree
was not built.

.. image:: Pictures/100000010000022D000000AEC32180F59FEAA725.png
   :width: 4.7354in
   :height: 1.4791in

#. 

   #. 

      #. 

         #. .. rubric:: FAIL_noephem_with_subscriptions
               :name: fail_noephem_with_subscriptions

This test is similar to *FAIL_noephem_only,* in that only the base *PointingRefFrame* and its
associated relative state are computed while still using Ephemeris-based frames. However, this test
specifically highlights the behavior that happens when a separate model, one dependent on either the
*Originating-Frame* or *Target-Frame*, activates and deactivates those same Ephemeris-based frames
internally without rebuilding the Ephemeris reference frame tree. These actions could interfere with
the assumption that the frames will be simply activated and apart of the Ephemeris tree at
initialization.

In this test, the input file acts as a "lurking model" that also depends on the Sun inertial frame.
At the start of the simulation, this lurking model adds the Sun inertial frame to the Ephemeris tree
and activates the frame. Although this setup is intended for the lurking model's own functionality,
it incidentally enables the *PointingRefFrame* to initialize successfully, since the required frame
is present in the Ephemeris tree at that time. After initialization, the lurking model deactivates
the Sun inertial frame, removing it from the Ephemeris tree. To avoid failure at this point, the
*PointingRefFrame* is also deactivated. Later in the simulation (at t=300000), the lurking model
reactivates the Sun inertial frame, and the *PointingRefFrame* is re-enabled to test its
functionality using that frame. However, the simulation fails at this point, because the Ephemeris
tree is not automatically rebuilt upon reactivation of the Sun inertial frame or *PointingRefFrame,*
unlike the *EphemBasedPointingRefFrame*. As a result, the Earth inertial and Sun inertial frame
remain disconnected in the Ephemeris tree, failing at the computation of the relative state between
the two frames. In conclusion, when using Ephemeris-based frames without the
*EphemBasedPointingRefFrame*, the Ephemeris tree must be rebuilt everytime the model is
re-subscribed to ensure the *Originating-Frame* and *Target-Frame* are dynamically connected (though
the recommended approach is to use *EphemBasedPointingRefFrame*, which handles this automatically).

.. image:: Pictures/1000000100000249000001052560A70AFB72E373.png
   :width: 4.9563in
   :height: 2.211in

.. |image1| image:: Pictures/100000010000018F000000DF06DF524173D534ED.png
   :width: 2.7846in
   :height: 1.5563in
.. |image2| image:: Pictures/1000000100000164000001061A269C53BDB88DDC.png
   :width: 2.1563in
   :height: 1.5571in
.. |image3| image:: Pictures/1000000100000083000000E0639094AA7CA31919.png
   :width: 0.9138in
   :height: 1.5634in
.. |image4| image:: Pictures/1000000100000195000000DC59D20B2409D92066.png
   :width: 2.1209in
   :height: 1.152in
.. |image5| image:: Pictures/1000000100000191000000DFAE2F859BC8FB20BE.png
   :width: 2.0717in
   :height: 1.152in
.. |image6| image:: Pictures/100000010000018E000000DDE662C22D71AD4852.png
   :width: 2.1283in
   :height: 1.1508in
.. |image7| image:: Pictures/10000001000001DD000000D1B52A837B6C2AAAE2.png
   :width: 3.2744in
   :height: 1.4346in
.. |image8| image:: Pictures/10000001000001D7000000D3A2360BA82E855597.png
   :width: 3.2028in
   :height: 1.4346in
.. |image9| image:: Pictures/10000001000001D4000000D1A4486562D6CF8642.png
   :width: 2.7366in
   :height: 1.2217in
.. |image10| image:: Pictures/10000001000001F9000000D87D9BDA5E781C8356.png
   :width: 2.8736in
   :height: 1.2291in
.. |image11| image:: Pictures/10000001000000BA000000D2943A32603F50BFB9.png
   :width: 1.0799in
   :height: 1.2189in
.. |image12| image:: Pictures/100000010000016C000000D293B645E09C668224.png
   :width: 2.6626in
   :height: 1.5362in
.. |image13| image:: Pictures/100000010000016D000000D0FDF51FB2554E0288.png
   :width: 2.6783in
   :height: 1.5264in
.. |image14| image:: Pictures/1000000100000026000000D0A4EF0DA57E0C341D.png
   :width: 0.2783in
   :height: 1.5228in
.. |image15| image:: Pictures/10000001000001DE000000D4B3E6157C5B19F5A8.png
   :width: 3.2744in
   :height: 1.4346in
.. |image16| image:: Pictures/10000001000001D5000000D291C83036BF198253.png
   :width: 3.2028in
   :height: 1.4346in
.. |image17| image:: Pictures/10000001000001D3000000D228CCAC1EDF41FBF1.png
   :width: 2.7366in
   :height: 1.2217in
.. |image18| image:: Pictures/10000001000001F9000000D1D73D577CD342CEE3.png
   :width: 2.8736in
   :height: 1.2291in
.. |image19| image:: Pictures/10000001000000B7000000D10074602451234B57.png
   :width: 1.0799in
   :height: 1.2189in
.. |image20| image:: Pictures/10000001000001DB000000D0D04940CD1EC9EAA4.png
   :width: 2.7437in
   :height: 1.2016in
.. |image21| image:: Pictures/1000000100000206000000D275FBE9EE2DD2FB7F.png
   :width: 2.9366in
   :height: 1.1902in
.. |image22| image:: Pictures/10000001000000AA000000D1CA2B3AECB0D45021.png
   :width: 0.9689in
   :height: 1.1902in
.. |image23| image:: Pictures/1000000100000041000000D23CCCD20D9B328391.png
   :width: 0.4465in
   :height: 1.4417in
.. |image24| image:: Pictures/10000001000001DC000000D01BF6C7A27A47C146.png
   :width: 3.2744in
   :height: 1.4346in
.. |image25| image:: Pictures/10000001000001D5000000D03E7F97D8318F7D81.png
   :width: 3.2028in
   :height: 1.4346in
.. |image26| image:: Pictures/10000001000001D2000000D2BCF442FB1DD84C2A.png
   :width: 2.7366in
   :height: 1.2217in
.. |image27| image:: Pictures/10000001000001F8000000D1C614C2C7D9AFB138.png
   :width: 2.8736in
   :height: 1.2291in
.. |image28| image:: Pictures/10000001000000BA000000D301764F87AA10E74D.png
   :width: 1.0799in
   :height: 1.2189in
.. |image29| image:: Pictures/1000000100000165000000D0F8A66EC2A63F5734.png
   :width: 2.6626in
   :height: 1.5362in
.. |image30| image:: Pictures/1000000100000160000000D15068E4826945D7FC.png
   :width: 2.6783in
   :height: 1.5264in
.. |image31| image:: Pictures/100000010000001E000000D1C0A6562BB1C7221E.png
   :width: 0.2783in
   :height: 1.5228in
