
# lift equation buried in MathUtil::compute_backard_difference
# input file. use this equation, as well as first and second
# derivatives of it, in validating the model classes

# define an equation
def funct(val):
  return 4*(val**3) - 3*(val**2) + 2*val + 1

# define the first derivative of equation
def fdfunct(val):
  return 12*(val**2) - 6*(val) + 2

# define the second derivative of equation
def sdfunct(val):
  return 24*(val) - 6


# vary the above equation to get another set of data...

# define equation2
def funct2(val):
  return 4*(val**3) + 3*(val**2) + 2*val + 1

# define the first derivative of equation2
def fdfunct2(val):
  return 12*(val**2) + 6*(val) + 2

# define the second derivative of equation2
def sdfunct2(val):
  return 24*(val) + 6

# vary the first equation to get another set of data...

# define equation3
def funct3(val):
  return 3*(val**3) + 4*(val**2) + 7*val + 1

# define the first derivative of equation3
def fdfunct3(val):
  return 9*(val**2) + 8*(val) + 7

# define the second derivative of equation3
def sdfunct3(val):
  return 18*(val) + 8

# Define a polynomial with vastly different coefficients
def funct4(val):
    return 6*(val**4) - 24*(val**3) + 2021*(val**2) + 340*(val) - 4967

def fdfunct4(val):
    return 24*(val**3) - 72*(val**2) + 4042*(val) + 340

def sdfunct4(val):
    return 72*(val**2) - 144*(val) + 4042
