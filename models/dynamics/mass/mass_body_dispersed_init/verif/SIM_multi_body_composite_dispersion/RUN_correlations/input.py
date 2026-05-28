# This run tests the appliation of correlations
# For example, with DispMatchXY, the y-value of the vector will be made to be
# a multiple of the x-value of the vector;
# the multiple is assigned by the variable *_sf
from Modified_data.setup import env_setup_no_log
env_setup_no_log(so, dynamics)

from Log_data.log_data import log_properties
dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)
log_properties( dr_group, "so.body[0].composite_properties")


def reset_values():
  so.adjust_0.nominal.mass = 1
  so.adjust_0.nominal.position = [1, 1, 1]
  so.adjust_0.nominal.inertia = [[1,1,1],[1,1,1],[1,1,1]]

  so.adjust_0.disperse.mass = 25
  so.adjust_0.disperse.position[0] = 5
  so.adjust_0.disperse.position[1] = 6
  so.adjust_0.disperse.position[2] = 7

  so.adjust_0.disperse.moi[0] = 2
  so.adjust_0.disperse.moi[1] = 3
  so.adjust_0.disperse.moi[2] = 4

  so.adjust_0.disperse.poi[0] = -1
  so.adjust_0.disperse.poi[1] = -2
  so.adjust_0.disperse.poi[2] = -3



# Test 0 -- do not run apply_correlation at all
trick.add_read(0.0, """
reset_values()
so.adjust_0.update( so.body[2])
""")

# Test 1 -- run apply_correlation with all scale factors at default 1
# Should match Test 0
trick.add_read(1.0,"""
reset_values()
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 2 -- increase all scale factors to 2
#           set correlation to y = 2x for pos, moi, poi
trick.add_read(2.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 2
so.adjust_0.disperse.moi_match_sf = 2
so.adjust_0.disperse.poi_match_sf = 2
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchYX
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchYX
so.adjust_0.disperse.poi_match = trick.MassPropertiesDispersions.DispMatchYX
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 3 -- increase all scale factors to 3
#           set correlation to z = 3x for pos, moi, poi
trick.add_read(3.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 3
so.adjust_0.disperse.moi_match_sf = 3
so.adjust_0.disperse.poi_match_sf = 3
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchZX
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchZX
so.adjust_0.disperse.poi_match = trick.MassPropertiesDispersions.DispMatchZX
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 4 -- increase all scale factors to 4
#           set correlation to z = 4y for pos, moi, poi
trick.add_read(4.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 4
so.adjust_0.disperse.moi_match_sf = 4
so.adjust_0.disperse.poi_match_sf = 4
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchZY
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchZY
so.adjust_0.disperse.poi_match = trick.MassPropertiesDispersions.DispMatchZY
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 5 -- increase all scale factors to 5
#           set correlation to y = z = 5x for pos, moi, poi
trick.add_read(5.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 5
so.adjust_0.disperse.moi_match_sf = 5
so.adjust_0.disperse.poi_match_sf = 5
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchZYX
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchZYX
so.adjust_0.disperse.poi_match = trick.MassPropertiesDispersions.DispMatchZYX
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 6 -- set all scale factors to unique values
#           set correlation to:
#              z = 2x for pos
#              z = 3y for moi
#              z = y  = 4x for poi
trick.add_read(6.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 2
so.adjust_0.disperse.moi_match_sf = 3 
so.adjust_0.disperse.poi_match_sf = 4
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchZX
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchZY
so.adjust_0.disperse.poi_match = trick.MassPropertiesDispersions.DispMatchZYX
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

# Test 7 -- set all scale factors to unique values
#           set correlation to:
#              z = 3y for pos
#              Off for moi
#              Invalid (Off) for poi
trick.add_read(7.0,"""
reset_values()
so.adjust_0.disperse.position_match_sf = 3
so.adjust_0.disperse.moi_match_sf = 4
so.adjust_0.disperse.poi_match_sf = 5
so.adjust_0.disperse.position_match = trick.MassPropertiesDispersions.DispMatchZY
so.adjust_0.disperse.moi_match = trick.MassPropertiesDispersions.DispMatchNone
so.adjust_0.disperse.poi_match = 500
so.adjust_0.disperse.apply_correlations()
so.adjust_0.update( so.body[2])
""")

trick.stop(7)
