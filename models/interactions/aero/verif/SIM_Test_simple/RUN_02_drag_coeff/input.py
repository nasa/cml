exec(open("input_common.py").read())
exec(open("Log_data/log_data.py").read())
aero.interface.executive.Aref = 2.0
aero.interface.executive.CD = 0.4
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.drag_coeff)
