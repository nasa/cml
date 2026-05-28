def log_add_3vec(group,var):
  for ii in range (0,3):
    group.add_variable(var + "[" + str(ii) + "]")
  return

def log_add_3x3mat(group,var):
  for ii in range (0,3):
    for jj in range (0,3):
      group.add_variable(var + "[" + str(ii) + "][" + str(jj) + "]")
  return
