# Position reference point on ECEF -y-axis.  Transformations should be:
#   ENU       ECEF
#   East       +x
#   North      +z
#   Up         -y
import math
exec(open("RUN_00_baseline/input.py").read())
log_add_ecef()
ts_so.evaluate_ecef = True
inv_sqrt2 = 1.0 / math.sqrt(2)
ts_so.ts.Q_enu_to_ecef.scalar = inv_sqrt2
ts_so.ts.Q_enu_to_ecef.vector = [inv_sqrt2, 0.0, 0.0]

dr_group.add_variable("ts_so.ts_y_in_ecef[1]")
dr_group.add_variable("ts_so.ts_y_in_ecef[0]")
dr_group.add_variable("ts_so.ts_x_in_ecef[1]")

trick.stop(30)
