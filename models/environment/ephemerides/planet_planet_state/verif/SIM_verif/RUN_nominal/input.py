exec(open("Log_data/log_rel_state.py").read())
exec(open("Modified_data/env_setup.py").read())

planet_rel_state.call_initialize = True
planet_rel_state.call_subscribe = True

planet_rel_state.relstate_name.from_frame_name = "Sun.inertial"
planet_rel_state.relstate_name.to_frame_name = "Earth.inertial"

trick.stop(1000.0)
