exec(open("input_common.py").read())
exec(open("Log_data/log_data.py").read())
aero.interface.executive.drag_mag = 1.2
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.constant_mag)
