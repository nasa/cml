exec(open("RUN_close_indefinite_duration_vent_on_depleted_mass/input.py").read())

# lower flowrate to give the tank enough time to vary mass for our checks.
# Does the vent(0) stop venting automatically when it gets past its stop time?
ventset_obj.set_2_tanks.get_vent(0).set_flowrate(0.1) # kg/s
ventset_obj.extra_vent.set_flowrate(0.1) # kg/s

trick.add_read(2.0, '''
ventset_obj.set_2_tanks.get_vent(0).set_duration(3.0)
''')

trick.stop(5)

