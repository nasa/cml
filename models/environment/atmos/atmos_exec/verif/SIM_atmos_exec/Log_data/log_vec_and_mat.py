def log_add_3vec(group,var):
  for ii in range (3):
    group.add_variable(var + "[" + str(ii) + "]")

def log_add_3x3mat(group,var):
  for ii in range (3):
    for jj in range (3):
      group.add_variable(var + "[" + str(ii) + "][" + str(jj) + "]")
