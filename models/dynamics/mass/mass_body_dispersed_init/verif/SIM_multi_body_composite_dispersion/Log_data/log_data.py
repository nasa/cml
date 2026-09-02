import trick


def log_properties(drg, prop_set):
  drg.add_variable( f"{prop_set}.mass")
  for ii in range(3):
    drg.add_variable(f"{prop_set}.position[{ii}]")
  for ii in range(3):
    for jj in range(3):
      drg.add_variable( f"{prop_set}.inertia[{ii}][{jj}]")

def log_body(drg, body):
  log_properties( drg, body+".composite_properties")
  log_properties( drg, body+".core_properties")



def log_default():
  dr_group = trick.DRAscii("test_data")
  dr_group.thisown = 0
  dr_group.set_cycle(1.0)
  dr_group.freq = trick.DR_Always
  trick.add_data_record_group(dr_group, trick.DR_Buffer)

  for ib in range(3):
    log_body( dr_group, f"so.body[{ib}]")
