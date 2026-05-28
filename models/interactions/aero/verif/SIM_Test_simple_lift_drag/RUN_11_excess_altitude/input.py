exec(open("input_common.py").read())

print("****** Testing excessive altitude ************")
aero.geodetic_altitude = 1e12

trick.stop(1)
