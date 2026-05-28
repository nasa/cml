exec(open("RUN_01_basic/input.py").read())
print("******************************************************************")
print("2 errors: sending NULL for both position and transformation matrix")
print("******************************************************************")

rocket_motor.null_dispersions()
trick.stop(0.01)
