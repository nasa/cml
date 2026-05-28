# Position reference point on ECEF x-axis.  Transformations should be:
#   ENU       ECEF
#   East       +y
#   North      +z
#   Up         +x
exec(open("RUN_00_baseline/input.py").read())
log_add_ecef()
ts_so.evaluate_ecef = True
ts_so.ts.Q_enu_to_ecef.scalar = 0.5
ts_so.ts.Q_enu_to_ecef.vector = [0.5, 0.5, 0.5]

dr_group.add_variable("ts_so.ts_x_in_ecef[2]")
dr_group.add_variable("ts_so.ts_y_in_ecef[0]")
dr_group.add_variable("ts_so.ts_z_in_ecef[1]")

trick.stop(30)
