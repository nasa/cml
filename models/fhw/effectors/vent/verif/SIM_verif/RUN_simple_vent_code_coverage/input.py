# This run simply exercises the trivial methods found in simple_vent.hh.

exec(open("Modified_data/env_setup.py").read())
ventset_obj.set_2_tanks.start_vents_at_activation = True
ventset_obj.extra_simple_vent.set_duration(5)
ventset_obj.set_2_tanks.get_vent(0).set_duration(5)

trick.add_read(1,"""
print("\\n"
"**************************************************************************\\n"
"start-time   :  %lf\\n"
"stop-time    :  %lf\\n"
"venting      :  %d\\n"
"flowrate-set :  %d\\n"
"exhaust-set  :  %d\\n"
"**************************************************************************"
%(ventset_obj.extra_simple_vent.get_start_time(),
  ventset_obj.extra_simple_vent.get_stop_time(),
  ventset_obj.extra_simple_vent.is_venting(),
  ventset_obj.extra_simple_vent.flowrate_is_set(),
  ventset_obj.extra_simple_vent.exhaust_speed_is_set()))
""")
trick.stop(1)
