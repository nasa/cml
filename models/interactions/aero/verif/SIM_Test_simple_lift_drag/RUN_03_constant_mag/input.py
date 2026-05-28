exec(open("input_common.py").read())
exec(open("Log_data/log_data.py").read())
aero.interface.executive.drag_mag = 1.2
aero.interface.executive.lift_mag = 0.6
aero.interface.executive.set_aero_type(trick.AeroExecutiveSimpleLiftDrag.constant_mag)
