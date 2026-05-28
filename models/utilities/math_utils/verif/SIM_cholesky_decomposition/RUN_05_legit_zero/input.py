exec(open("RUN_01_baseline/input.py").read())
for ii in range (10):
  simple_so.A[ii][3] = 0.0
  simple_so.A[3][ii] = 0.0
