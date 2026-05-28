exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_base.py").read())
earth.planet.pfix.state.rot.ang_vel_this = [0.0, 0.0, 7.2921151467063882e-05]
vehicle.planet_rel_state.subscribe()
trick.stop(100)
