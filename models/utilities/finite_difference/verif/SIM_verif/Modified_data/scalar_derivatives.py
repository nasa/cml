import finite_difference

def scalar_sd(self, dt_incr, max_dt, function, sdfunction):
  sd = trick.ScalarSecondDerivative()
  sd_py = finite_difference.ScalarSecondDerivative()

  t = 0.0
  dt = 0.0
  limit = 10.0005
  while t < limit:
    is_good, deriv = sd.compute(function(t), t)
    is_good_py, deriv_py = sd_py.compute(function(t), t)

    if is_good:
      # Check deriv vs python
      self.assertEqual(is_good, is_good_py)
      self.assertAlmostEqual(deriv, deriv_py)

    # increment dt
    dt += dt_incr
    # if dt exceeds max_dt,
    if (dt > max_dt):
      # reset it back to dt_incr
      dt = dt_incr

    # increment time by dt
    t += dt

def scalar_fd(self, dt_incr, max_dt, function, fdfunction):
  fd = trick.ScalarFirstDerivative()
  fd_py = finite_difference.ScalarFirstDerivative()

  t = 0.0
  dt = 0.0
  limit = 10.0005
  while t < limit:
    is_good, deriv = fd.compute(function(t), t)
    is_good_py, deriv_py = fd_py.compute(function(t), t)

    if is_good:
      # Check model vs python
      self.assertEqual(is_good, is_good_py)
      self.assertAlmostEqual(deriv, deriv_py)

    # increment dt
    dt += dt_incr
    # if dt exceeds max_dt,
    if (dt > max_dt):
      # reset it back to dt_incr
      dt = dt_incr

    # increment time by dt
    t += dt

# NOTE: use sin() function since it returns a value
# with a ton of significant digits!

# define this one explicitely since the second derivative
# of sin() is -sin().

def scalar_sd_sine(self, dt_incr, max_dt):
  sd = trick.ScalarSecondDerivative()
  sd_py = finite_difference.ScalarSecondDerivative()

  t = 0.0
  dt = 0.0
  limit = 10.0005
  while t < limit:
    is_good, deriv = sd.compute(math.sin(t), t)
    is_good_py, deriv_py = sd_py.compute(math.sin(t), t)

    if is_good:
      # Check deriv vs python
      self.assertEqual(is_good, is_good_py)
      self.assertAlmostEqual(deriv, deriv_py)

    # increment dt
    dt += dt_incr
    # if dt exceeds max_dt,
    if (dt > max_dt):
      # reset it back to dt_incr
      dt = dt_incr

    # increment time by dt
    t += dt
