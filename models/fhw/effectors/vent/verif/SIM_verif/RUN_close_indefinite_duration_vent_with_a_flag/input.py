exec(open("RUN_close_indefinite_duration_vent_on_depleted_mass/input.py").read())

# lower flowrate to give the tank enough time to vary mass for our checks.
# Does the extra vent stop venting automatically the indefinite duration 
# flag is reset to false?
ventset_obj.set_2_tanks.get_vent(0).set_flowrate(0.1) # kg/s
ventset_obj.extra_vent.set_flowrate(0.1) # kg/s

trick.add_read(5,"ventset_obj.extra_vent.indefinite_duration = False")
trick.stop(5)
