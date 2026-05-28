# Set a trick event to deactivate the model 50 seconds into the run
trick.add_read(50.0,"""
veh1.atmos_exec.unsubscribe()
veh2.atmos_exec.unsubscribe()
""")
trick.stop(100.0)
