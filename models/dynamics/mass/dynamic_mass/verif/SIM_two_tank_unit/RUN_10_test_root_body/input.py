exec(open("RUN_01_parallel_tanks/input.py").read())
log_countdown()

# The purpose of this test is to verify the countdown clock to needing to test
#  the root-body.
# The test of the root-body has already been captured elsewhere.
mass_test.group.countdown_reset = 3
trick.stop(10)
