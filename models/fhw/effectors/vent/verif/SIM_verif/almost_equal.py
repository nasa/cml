import numpy
def almostEqual(val1, val2):
  # need to check if signs between input values match. need to
  # do this before taking absolute values of inputs later on
  if (numpy.sign(val1) != numpy.sign(val2)):
    print("*** almostEqual(%f, %f) => signs of input values do not match ***"%(val1,val2))
    print("    cannot continue!!!")
    return False
  diff = abs(val1) - abs(val2)
  almostZero = abs(diff) < 0.000001
  #print("almostEqual(%f,%f) => difference = %f, almost equal = %d"%(val1, val2, diff, almostZero))
  return (almostZero)
