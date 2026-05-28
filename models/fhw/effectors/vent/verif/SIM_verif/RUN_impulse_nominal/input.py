# Check the application of an impulse
exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_vents.py").read())
log_add_state()

# Move the vent off-axis to generate an impulsive moment.
ventset_obj.set_2_tanks.get_vent(1).location[0] = 1.0

trick.add_read(1,"""
print("\\n"
"***************************************************************************\\n"
"vent-body is root-body : %d\\n"
"***************************************************************************\\n"
%ventset_obj.vent_body.is_root_body())
ventset_obj.set_2_tanks.start_vent(1)
""")
trick.stop(3)
