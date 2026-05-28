# This unit test checks for a legitimate unit-vector specification
exec(open("RUN_02_not_active/input.py").read())
rocket_motor.basic_motor.commanded = True
rocket_motor.basic_motor.thrust_unit_motor = [1.0, 2.0, 3.0]
print("**********************************************")
print("Command motor with a non-unit direction vector")
print("**********************************************")
trick.stop(0.01)
