# This run executes like RUN_02, except the moments are disabled.
# The variables:
#  - output.torque
#  - output.moment_mrc
# should be zero throughout.
exec(open("RUN_02_example_table/input.py").read())
for ii in range(3):
  drg1.add_variable("aero.interface.output.moment_mrc[%d]"%ii);


aero.interface.disable_aero_moments = True
