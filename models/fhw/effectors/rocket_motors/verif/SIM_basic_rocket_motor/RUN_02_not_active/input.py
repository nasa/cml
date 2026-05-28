# This run does nothing; the motor is not commanded.
exec(open("RUN_01_basic/input.py").read())

rocket_motor.basic_motor.commanded = False
