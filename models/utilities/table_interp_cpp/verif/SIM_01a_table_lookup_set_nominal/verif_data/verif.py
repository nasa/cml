import csv

###############################################################################
# Find the index of the tabulated value that matches within 1E-12 of the
# current value, or return -1 if no match found
###############################################################################
def match( val, domain):
  for ii in range(len(domain)):
    if abs(val - domain[ii]) < 1E-12:
      return ii
  return -1

###############################################################################
# Find the index of the tabulated value to the immediate left of the
# current value
###############################################################################
def left_index( val, domain, increasing):
  # returns left-index, starting at 0
  # return -1 if off-table-left
  # return len(domain)-1 if off-table-right
  dom_sz = len(domain);
  if increasing:
    for ii in range(dom_sz):
      if val <= domain[ii]:
        return ii-1
  else:
    for ii in range(dom_sz):
      if val >= domain[ii]:
        return ii-1
  return dom_sz-1


###############################################################################
# Find the index of the tabulated value immediately left of the
# current value
###############################################################################
def prev( val, domain, increasing=True):
  ix = match( val, domain)
  if ix >= 0:
    return ix
  ix = left_index( val, domain, increasing)
  if ix == -1:
    ix = 0
  return ix

###############################################################################
# Find the index of the tabulated value immediately right of the
# current value
###############################################################################
def nextix( val, domain, increasing=True):

  ix = match( val, domain)
  if ix >= 0:
    return ix

  ix = left_index( val, domain, increasing)+1
  if ix > len(domain)-1:
    ix = len(domain)-1
  return ix

###############################################################################
# Find the index of the tabulated value immediately less than the
# current value
###############################################################################
def floor( val, domain, increasing=True):

  ix = match( val, domain)
  if ix >= 0:
    return ix

  ix = left_index( val, domain, increasing)
  if ix == -1:
    ix = 0
  elif ix < len(domain)-1 and not increasing:
    ix = ix+1
  return ix

###############################################################################
# Find the index of the tabulated value immediately greater than the
# current value
###############################################################################
def ceiling( val, domain, increasing=True):

  ix = match( val, domain)
  if ix >= 0:
    return ix

  ix = left_index( val, domain, increasing)
  if ix == -1:
    ix = 0
  elif ix < len(domain)-1 and increasing:
    ix = ix+1
  return ix

###############################################################################
# Find the index of the tabulated value closest to the
# current value
###############################################################################
def roundix( val, domain, increasing=True):

  ix = match( val, domain)
  if ix >= 0:
    return ix

  ix = left_index( val, domain, increasing)
  if ix == -1:
    ix = 0
  elif ix < len(domain)-1 and abs(val - domain[ix+1]) < abs(val - domain[ix]):
    ix = ix+1
  return ix

###############################################################################
# Find the fractional index describing the location of the current value as
# being the position between the tabulated value to the immediate left of the
# current value and the fractional location between that and the tabulated
# value to the immediate right of the current value.
###############################################################################
def interp( val, domain, increasing=True):

  ix = match( val, domain)
  if ix >= 0:
    return ix

  ix = left_index( val, domain, increasing)
  if ix == -1:
    ix = 0
  elif ix < len(domain)-1:
    if increasing:
      ix = ix + (val - domain[ix]) / (domain[ix+1] - domain[ix])
    else:
      ix = ix + 1 + (domain[ix+1] - val) / (domain[ix] - domain[ix+1])
  return ix

###############################################################################
# Check the values computed here against the sim output
###############################################################################
def check_values(ix1, ix2, ix3, ix4, line, indeps, sim_val):
    # Check so_defined values against the computed values
    delta = [0,0,0]
    delta[0] = 1000 + ix1*100+ix2*10+ix3 - float(sim_val[16])
    delta[1] = 2000 + ix1*100+ix2*10+ix3 - float(sim_val[17])
    delta[2] = 3000 + ix1*100+ix2*10+ix4 - float(sim_val[18])
    if max(delta) > 1E-10 or min(delta) < -1E-10:
      print(' ')
      print('deltas at line :',line, delta)
      print('independent values :', \
             float(sim_val[indeps[0]]), \
             float(sim_val[indeps[1]]), \
             float(sim_val[indeps[2]]), \
             float(sim_val[indeps[3]]))
      print('sim   indices      :', \
             float(sim_val[2])+float(sim_val[3]), \
             float(sim_val[8])+float(sim_val[9]), \
             float(sim_val[11])+float(sim_val[12]), \
             float(sim_val[14])+float(sim_val[15]))
      print('verif indices      :', ix1,ix2,ix3,ix4)
      print('sim values         :',\
             float(sim_val[16]),\
             float(sim_val[17]),\
             float(sim_val[18]))
      print('verif values       :',\
             1000 + ix1*100+ix2*10+ix3,\
             2000 + ix1*100+ix2*10+ix3,\
             3000 + ix1*100+ix2*10+ix4)
    # check so_defined values against on-the-fly values
    delta[0] = float(sim_val[16]) - float(sim_val[19])
    delta[1] = float(sim_val[17]) - float(sim_val[20])
    delta[2] = float(sim_val[18]) - float(sim_val[21])
    if max(delta) > 1E-10 or min(delta) < -1E-10:
      print(' ')
      print('deltas between two table-sets found at line :',line, delta)
      print('so-defined table values :', \
             float(sim_val[16]), \
             float(sim_val[17]), \
             float(sim_val[18]))
      print('on-the-fly table values :', \
             float(sim_val[19]), \
             float(sim_val[20]), \
             float(sim_val[21]))





###############################################################################
# Main script
###############################################################################
calib0 = [9,8,-2]
calib1 = [2,4,6,8]
calib2 = [2,4,6]

indeps = [1,7,10,13]

# ceil-int-int-int
with open('RUN_ceil_int_int_int/log_test_data.csv') as datafile:
  print('************* ceil-int-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = ceiling( float(test[indeps[0]]), calib0, False)
    ix2 = interp( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) % \
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# floor-int-int-int
with open('RUN_floor_int_int_int/log_test_data.csv') as datafile:
  print('************* floor-int-int-int    **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = floor( float(test[indeps[0]]), calib0, False)
    ix2 = interp( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-ceil-int-int
with open('RUN_int_ceil_int_int/log_test_data.csv') as datafile:
  print('************* int-ceil-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = ceiling( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-floor-int-int
with open('RUN_int_floor_int_int/log_test_data.csv') as datafile:
  print('************* int-floor-int-int    **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = floor( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-next-int-int
with open('RUN_int_next_int_int/log_test_data.csv') as datafile:
  print('************* int-next-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = nextix( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-prev-int-int
with open('RUN_int_prev_int_int/log_test_data.csv') as datafile:
  print('************* int-prev-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = prev( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-round-int-int
with open('RUN_int_round_int_int/log_test_data.csv') as datafile:
  print('************* int-round-int-int    **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = roundix( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# int-int-int-int
with open('RUN_interp_all/log_test_data.csv') as datafile:
  print('************* int-int-int-int      **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = interp( float(test[indeps[0]]), calib0, False)
    ix2 = interp( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# next-int-int-int
with open('RUN_next_int_int_int/log_test_data.csv') as datafile:
  print('************* next-int-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = nextix( float(test[indeps[0]]), calib0, False)
    ix2 = interp( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# prev-int-int-int
with open('RUN_prev_int_int_int/log_test_data.csv') as datafile:
  print('************* prev-int-int-int     **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = prev( float(test[indeps[0]]), calib0, False)
    ix2 = interp( float(test[indeps[1]]), calib1)
    ix3 = interp( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = interp(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)

# prev-next-floor-ceil
with open('RUN_prev_next_floor_ceil/log_test_data.csv') as datafile:
  print('************* prev-next-floor-ceil **************')
  data_ciii = csv.reader(datafile, delimiter=',')
  next(data_ciii, None) # skip first line

  line = 1
  for test in data_ciii:
    line += 1
    ix1 = prev( float(test[indeps[0]]), calib0, False)
    ix2 = nextix( float(test[indeps[1]]), calib1)
    ix3 = floor( float(test[indeps[2]]), calib2)
    wrapped_val = calib2[0] + \
                  ((float(test[indeps[2]]) - calib2[0]) %
                   (calib2[-1] - calib2[0]))
    ix4 = ceiling(wrapped_val, calib2)
    check_values( ix1, ix2, ix3, ix4, line, indeps, test)
