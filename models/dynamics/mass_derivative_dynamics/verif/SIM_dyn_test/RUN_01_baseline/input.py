exec(open("RUN_00_off/input.py").read())

# uncomment this line to pick up cases presented in document
# drg1.set_cycle(0.005)

vehicle.test_dyn.subscribe()
trick.stop(10.0)
