exec(open("RUN_conical/input.py").read())

# Change the shadow geometry from conical to cylindrical. The illumination
# fraction will immediately jump between 1 and 0 instead of changing gradually.
vehicle.eclipse.set_earth_shadow_geometry(trick.RadiationThirdBody.Cylindrical)
# Changing the Moon's shadow geometry has no effect but is included for code
# coverage.
vehicle.eclipse.set_moon_shadow_geometry(trick.RadiationThirdBody.Cylindrical)
