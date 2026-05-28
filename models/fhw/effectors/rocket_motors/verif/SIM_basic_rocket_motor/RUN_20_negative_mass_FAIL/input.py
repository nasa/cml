# This unit test is specifically used to hit the check in
# rocket_motor_basic.cc::initialize that makes sure the propellant mass in
# the model isn't < 0 as that should be impossible. If it is hit the sim
# fails and prints back a message to the user.

# In this case the consumable mass is set to -1 kg; this value being
# negative is interpreted by the Dynamic-mass model as meaning "unset".
# Consequently, the consumable-mass is computed from the difference of the
# core-mass and the residual-mass.  In this case, 0-2 = -2.
# value entered for the mass to ensure it is > 0kg.

# The Dynamic-mass model will flag an error to this effect before this
# model terminates the sim.

exec(open("RUN_01_basic/input.py").read())
rocket_motor.mass_init.mass =0.0
rocket_motor.motor_prop.residual_mass = 2
rocket_motor.motor_prop.dynamic_properties.consumable_mass = -1
print("***********************************")
print("Generating negative mass.  Terminal")
print("***********************************")
