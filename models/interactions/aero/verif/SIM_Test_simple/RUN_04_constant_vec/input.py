exec(open("input_common.py").read())
exec(open("Log_data/log_data.py").read())
aero.interface.output.force = [1.1, 1.2, 1.3]
aero.interface.executive.set_drag_type( trick.AeroExecutiveSimple.constant_vec)
