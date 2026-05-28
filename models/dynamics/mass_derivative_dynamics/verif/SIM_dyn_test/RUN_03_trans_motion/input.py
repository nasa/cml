exec(open("RUN_01_baseline/input.py").read())
log_basic_addon()

vehicle.nozzle_position = [1.8, 0 ,0]
vehicle.mass_consumption = 0.4
# Alternative value; uncomment this line for more rotations.
#vehicle.mass_consumption = 0.1
trick.stop(25.0)
