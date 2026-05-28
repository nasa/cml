# Tests the correlated dispersion of pos, vel, and att
# Random number generator does not generate new random numbers, so on 2nd
# dispersion, the same errors should be re-applied.

exec(open("RUN_inertial_pva_PVA/input.py").read())

trick.add_read(2,"verif.corr_state.hold_previous_random_vec = True")
