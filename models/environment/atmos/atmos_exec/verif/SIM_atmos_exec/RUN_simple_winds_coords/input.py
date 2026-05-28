exec(open("RUN_simple_winds_components/input.py").read())
veh1.atmos_exec.winds_simple.set_altitude_type(trick.AtmosphereExec_SimpleLookupWind.Topocentric)
veh1.atmos_exec.winds_simple.wind_components_type = trick.AtmosphereExec_SimpleLookupWind.Topocentric
veh2.atmos_exec.winds_simple.set_altitude_type(trick.AtmosphereExec_SimpleLookupWind.Spherical)
trick.add_read(500.0, """
veh1.atmos_exec.winds_simple.set_altitude_type(trick.AtmosphereExec_SimpleLookupWind.Topodetic)
veh1.atmos_exec.winds_simple.wind_components_type = trick.AtmosphereExec_SimpleLookupWind.Topodetic
veh2.atmos_exec.winds_simple.set_altitude_type(trick.AtmosphereExec_SimpleLookupWind.Topocentric)
veh2.atmos_exec.winds_simple.wind_components_type = trick.AtmosphereExec_SimpleLookupWind.Topocentric
""")
