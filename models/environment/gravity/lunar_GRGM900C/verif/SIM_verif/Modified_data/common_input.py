import math
exec(open("Log_data/log_data.py").read())

dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4
dynamics.dyn_manager_init.mode = trick.DynManagerInit.EphemerisMode_SinglePlanet
dynamics.dyn_manager_init.central_point_name = "Moon"

vehicle.body.integ_frame_name = "Moon.inertial"
vehicle.body.set_name("test_vehicle")

# Gravity
vehicle.grav_controls.source_name  = "Moon"
vehicle.grav_controls.active       = True
vehicle.grav_controls.gradient     = False
def set_gravity( degree, order):
  vehicle.grav_controls.spherical = (degree == 0 and order == 0)
  vehicle.grav_controls.degree = degree
  vehicle.grav_controls.order = order
set_gravity(0,0) #spherical
vehicle.body.add_control( vehicle.grav_controls )


def add_lat_lon_sweep( angle_deg):
  vehicle.sweep_longitude(  0,
                            2*math.pi,
                            math.radians(angle_deg))
  vehicle.sweep_latitude( -math.pi/2,
                           math.pi/2,
                           math.radians(angle_deg))
  trick.stop((361/angle_deg)*(181/angle_deg))

# produce a radius sweep for each of 12 pts around the globe
def rad_sweep_12pt( radius_lo, radius_hi, radius_step):
  vehicle.sweep_radius( radius_lo,
                        radius_hi,
                        radius_step)
  vehicle.sweep_longitude( -math.pi,
                            math.pi/2,
                            math.pi/2)
  vehicle.sweep_latitude( -math.pi/4,
                           math.pi/4,
                           math.pi/4)

def high_alt_sweep():
  rad_sweep_12pt( 4E6, 10E6, 1E6)
  trick.stop(84)

def low_alt_sweep():
  rad_sweep_12pt( 1.8E6, 3.0E6, 0.2E6)
  trick.stop(84)
