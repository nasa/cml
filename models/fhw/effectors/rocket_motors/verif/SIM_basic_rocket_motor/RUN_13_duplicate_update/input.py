#This run simply duplicates the update call at t=0
exec(open ("RUN_01_basic/input.py").read())
print("***************************************")
print(" No error message")
print("***************************************")
trick.add_read(0, """
rocket_motor.basic_motor.update()
print("***************************************")
print("current mass is",rocket_motor.basic_motor.get_mass(),"kg")
print("***************************************")
""")

trick.stop(0.01)
