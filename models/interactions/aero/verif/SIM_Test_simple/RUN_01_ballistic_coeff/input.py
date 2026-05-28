exec(open("input_common.py").read())
exec(open("Log_data/log_data.py").read())
aero.mass = 3.0
aero.interface.executive.BC = 2.0
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.ballistic_coeff)
