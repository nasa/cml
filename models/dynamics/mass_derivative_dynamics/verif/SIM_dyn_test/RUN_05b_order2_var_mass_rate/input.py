exec(open("RUN_01_baseline/input.py").read())
log_basic_addon()
drg1.set_cycle(0.005)

# Make the mass-loss oscillate and double the amplitude so that it averages to
# the same value.
vehicle.mass_cons_freq = 100
vehicle.mass_consumption = .2
