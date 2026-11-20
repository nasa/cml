# fill out log variables under here
import numpy as np
exec(open("Log_data/log_two_level_vent.py").read())

# Vehicle Body Properties
two_level_vent_model_obj.vehicle_body.mass.composite_properties.mass = 100.0 # WAG for Mass,  need to add inertia, center-of-mass, orbital state (r,v), attitude state (q, w)?
two_level_vent_model_obj.vehicle_body.mass.composite_properties.position[0] = 0.0
two_level_vent_model_obj.vehicle_body.mass.composite_properties.position[1] = 0.0
two_level_vent_model_obj.vehicle_body.mass.composite_properties.position[2] = 0.0

# --------------------- #
# Default Vent Properties #
# --------------------- #

# ------------------------------ #
# Vent Set 1: Independent Vents #
# ------------------------------ #

# Vent-Specific Parameters
two_level_vent_model_obj.vent1.name                     = "IDP_Vent1"
two_level_vent_model_obj.vent1.accel_mag_high           = 2.0
two_level_vent_model_obj.vent1.accel_mag_low            = 1.0
two_level_vent_model_obj.vent1.high_duration_randFlag   = False
two_level_vent_model_obj.vent1.high_duration            = 2.0
two_level_vent_model_obj.vent1.wait_time_randFlag       = False
two_level_vent_model_obj.vent1.high_wait_time           = 5.0
two_level_vent_model_obj.vent1.dv_max                   = two_level_vent_model_obj.vent1.accel_mag_high*two_level_vent_model_obj.vent1.high_duration
two_level_vent_model_obj.vent1.indefinite_duration      = True

two_level_vent_model_obj.vent2.name                     = "IDP_Vent2"
two_level_vent_model_obj.vent2.accel_mag_high           = 3.0
two_level_vent_model_obj.vent2.accel_mag_low            = 1.0
two_level_vent_model_obj.vent2.high_duration_randFlag   = False
two_level_vent_model_obj.vent2.high_duration            = 2.0
two_level_vent_model_obj.vent2.wait_time_randFlag       = False
two_level_vent_model_obj.vent2.high_wait_time           = 5.0
two_level_vent_model_obj.vent2.dv_max                   = two_level_vent_model_obj.vent2.accel_mag_high*two_level_vent_model_obj.vent2.high_duration
two_level_vent_model_obj.vent2.indefinite_duration      = True


venting_dir = [0.0, 0.0, 0.0]

venting_dir[0] = 1.0
venting_dir[1] = 0.0
venting_dir[2] = 0.0
two_level_vent_model_obj.vent1.set_direction(venting_dir)

venting_dir[0] = 0.0
venting_dir[1] = 1.0
venting_dir[2] = 0.0
two_level_vent_model_obj.vent2.set_direction(venting_dir)


# Position of Vent in Structural Frame [m]
two_level_vent_model_obj.vent1.location[0] = 0.0
two_level_vent_model_obj.vent1.location[1] = 1.0
two_level_vent_model_obj.vent1.location[2] = 0.0

two_level_vent_model_obj.vent2.location[0] = 0.0
two_level_vent_model_obj.vent2.location[1] = 0.0
two_level_vent_model_obj.vent2.location[2] = 1.0


# --------------------- #
# Default Vent Properties #
# --------------------- #

# ---------------------------------- #
# Vent Set 2: Custom Number of Vents #
# ---------------------------------- #

for ii in range(6):
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).accel_mag_high           = 2.0
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).accel_mag_low            = 1.0
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).high_duration_randFlag   = False
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).high_duration            = 2.0
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).wait_time_randFlag       = False
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).high_wait_time           = 5.0
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).dv_max                   = \
        two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).accel_mag_high*two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).high_duration
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).indefinite_duration      = True
    venting_dir[0] = 1.0
    venting_dir[1] = 0.0
    venting_dir[2] = 0.0
    two_level_vent_model_obj.two_level_vent_set2.get_two_level_vent(ii).set_direction(venting_dir)

two_level_vent_model_obj.two_level_vent_set1.subscribe()
two_level_vent_model_obj.two_level_vent_set2.subscribe()
