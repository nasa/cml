import sys
import unittest
import math
import finite_difference

exec(open("Modified_data/fake_sys_argv.py").read())
exec(open("Modified_data/functions.py").read())
exec(open("Modified_data/scalar_derivatives.py").read())


# This test case uses sin() function because it returns
# a value with a ton of significant digits!

class Test(unittest.TestCase):
  # goes from 0.0 to -10.0. internally decrementing time
  # so you must pass in a positive dt_incr!
  def scalar_negative_time(self, dt_incr, max_dt, derivative = 'first'):
    if derivative == "first":
      d_class = trick.ScalarFirstDerivative()
      d_class_py = finite_difference.ScalarFirstDerivative()
    else:
      # Assume derivative == 'second'
      d_class = trick.ScalarSecondDerivative()
      d_class_py = finite_difference.ScalarSecondDerivative()
  
    t = 0.0
    dt = 0.0
    limit = -10.0005
    while t > limit:
      is_good, deriv = d_class.compute(math.sin(t), t)
      is_good_py, deriv_py = d_class_py.compute(math.sin(t), t)
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
  
      # decrement time by dt
      t -= dt

  def test1_scalar_first_derivative(self):
    fd = trick.ScalarFirstDerivative()
    fd_py = finite_difference.ScalarFirstDerivative()

    # Model
    is_good, deriv = fd.compute(math.sin(0.0), 0.0)

    # Python
    is_good_py, deriv_py = fd_py.compute(math.sin(0.0), 0.0)

    # Compare!
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)

    # Repeat after stepping the time up
    is_good, deriv = fd.compute(math.sin(0.1), 0.1)
    is_good_py, deriv_py = fd_py.compute(math.sin(0.1), 0.1)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)

    # first derivative of the sin(x) is cos(x)
    is_good, deriv = fd.compute(math.sin(0.2), 0.2)
    is_good_py, deriv_py = fd_py.compute(math.sin(0.2), 0.2)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)

    def rms_error(dt):
      fd = trick.ScalarFirstDerivative()
      fd_py = finite_difference.ScalarFirstDerivative()
      t = 0.0
      error = 0.0
      count = 0
      while t < 10.0:
        is_good, deriv = fd.compute(math.sin(t), t)
        is_good_py, deriv_py = fd_py.compute(math.sin(t), t)
        truth = math.cos(t)
        if is_good > 0:
          # Check model vs python
          self.assertEqual(is_good, is_good_py)
          self.assertAlmostEqual(deriv, deriv_py)
          count += 1
          error += (truth - deriv)**2
        t += dt
      return math.sqrt(error / count)

    large_error = rms_error(0.1)
    small_error = rms_error(0.05)

    # The error of a second order method should reduce quadratically.
    self.assertTrue(small_error < (large_error / 4) * 1.1)

    [scalar_fd(self, 10**x, 10**x, math.sin, math.cos) for x in [-2, -3, -4]]

    [scalar_fd(self, 0.1, x/10, math.sin, math.cos) for x in range(2,6)]

    [scalar_fd(self, 0.01, x/100, math.sin, math.cos) for x in range(2,11)]

    [scalar_fd(self, 0.001, x/1000, math.sin, math.cos) for x in [2,5,10,15]]

    [scalar_fd(self, 0.003, x/1000, math.sin, math.cos) for x in [6,10,15]]

    [scalar_fd(self, 0.005, x/1000, math.sin, math.cos) for x in range(10,30,5)]
    
    [scalar_fd(self, 0.0001, x/10000, math.sin, math.cos) for x in [2,3,6]]

  def test2_scalar_first_derivative_backward_time(self):
    fd = trick.ScalarFirstDerivative()
    fd_py = finite_difference.ScalarFirstDerivative()

    is_good, deriv = fd.compute(math.sin(0.0), 0.0)
    is_good_py, deriv_py = fd_py.compute(math.sin(0.0), 0.0)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = fd.compute(math.sin(-0.1), -0.1)
    is_good_py, deriv_py = fd_py.compute(math.sin(-0.1), -0.1)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = fd.compute(math.sin(-0.2), -0.2)
    is_good_py, deriv_py = fd_py.compute(math.sin(-0.2), -0.2)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)

    # backward linear and non-linear time tests
    num_power = [-1, -2, -3]
    num_scale = [1, 2, 3, 5]
    [self.scalar_negative_time(10**x,i*10**x) for x in num_power for i in num_scale]

  def test3_first_derivative_function(self):

    nums = [0.1, 0.01, 0.001, 0.003, 0.005]
    [scalar_fd(self, x, x, funct, fdfunct) for x in nums]

    [scalar_fd(self, 0.1, x/10, funct, fdfunct) for x in range(2,6)]

    [scalar_fd(self, 0.01, x/100, funct, fdfunct) for x in range(2,11)]

    [scalar_fd(self, 0.001, x/1000, funct, fdfunct) for x in [2,5,10,15]]

    [scalar_fd(self, 0.003, x/1000, funct, fdfunct) for x in [6,10,15]]

    [scalar_fd(self, 0.005, x/1000, funct, fdfunct) for x in range(10,30,5)]

    [scalar_fd(self, 0.0001, x/10000, funct, fdfunct) for x in [2,3,6]]

  def test4_scalar_second_derivative(self):
    sd = trick.ScalarSecondDerivative()
    sd_py = finite_difference.ScalarSecondDerivative()

    is_good, deriv = sd.compute(math.sin(0.0), 0.0)
    is_good_py, deriv_py = sd_py.compute(math.sin(0.0), 0.0)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = sd.compute(math.sin(0.01), 0.01)
    is_good_py, deriv_py = sd_py.compute(math.sin(0.01), 0.01)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = sd.compute(math.sin(0.02), 0.02)
    is_good_py, deriv_py = sd_py.compute(math.sin(0.02), 0.02)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    # second derivative of the sin(x) is:
    # derivative of the derivative of sin(x).
    # since derivative of sin(x) is cos(x),
    # the derivative of cos(x) is -sin(x).
    is_good, deriv = sd.compute(math.sin(0.03), 0.03)
    is_good_py, deriv_py = sd_py.compute(math.sin(0.03), 0.03)
    
    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    def rms_error(dt):
      sd = trick.ScalarSecondDerivative()
      sd_py = finite_difference.ScalarSecondDerivative()

      t = 0.0
      error = 0.0
      count = 0
      while t < 10.0:
        is_good, deriv = sd.compute(math.sin(t), t)
        is_good_py, deriv_py = sd_py.compute(math.sin(t), t)

        truth = -math.sin(t)
        if is_good > 0:
          # Check the model vs python
          self.assertEqual(is_good, is_good_py)
          self.assertAlmostEqual(deriv, deriv_py)

          count += 1
          error += (truth - deriv)**2
        t += dt
      return math.sqrt(error / count)

    large_error = rms_error(0.01)
    small_error = rms_error(0.005)

    # The error of a second order method should reduce quadratically.
    self.assertTrue(small_error < (large_error / 4) * 1.1)

    [scalar_sd_sine(self, 10**i, 10**i) for i in [-1, -2, -3, -4]]

    [scalar_sd_sine(self, 0.1, x/10) for x in range(2, 6)]

    [scalar_sd_sine(self, 0.01, x/100) for x in range(2, 11)]

    [scalar_sd_sine(self, 0.001, x/1000) for x in [2, 5, 10, 15]]

    [scalar_sd_sine(self, 0.003, x/1000) for x in [6, 10, 15]]

    [scalar_sd_sine(self, 0.005, x/1000) for x in range(10,30,5)]
    
    [scalar_sd_sine(self, 0.0001, x/10000) for x in [2, 3, 6]]

  def test5_scalar_second_derivative_backward_time(self):
    sd = trick.ScalarSecondDerivative()
    sd_py = finite_difference.ScalarSecondDerivative()

    is_good, deriv = sd.compute(math.sin(0.0), 0.0)
    is_good_py, deriv_py = sd_py.compute(math.sin(0.0), 0.0)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = sd.compute(math.sin(-0.1), -0.1)
    is_good_py, deriv_py = sd_py.compute(math.sin(-0.1), -0.1)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = sd.compute(math.sin(-0.2), -0.2)
    is_good_py, deriv_py = sd_py.compute(math.sin(-0.2), -0.2)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    is_good, deriv = sd.compute(math.sin(-0.3), -0.3)
    is_good_py, deriv_py = sd_py.compute(math.sin(-0.3), -0.3)

    self.assertEqual(is_good, is_good_py)
    self.assertAlmostEqual(deriv, deriv_py)
    
    # backward linear and non-linear time tests
    [self.scalar_negative_time(10**x, i*10**x, derivative='second') for x in [-1, -2, -3] for i in [1,2,3,5]]

  def test6_second_derivative_function(self):

    nums = [0.1, 0.01, 0.001, 0.003, 0.005, 0.0001]
    [scalar_sd(self, x, x, funct, sdfunct) for x in nums]

    [scalar_sd(self, 0.1, x/10, funct, sdfunct) for x in range(2,6)]

    [scalar_sd(self, 0.01, x/100, funct, sdfunct) for x in range(2,11)]

    [scalar_sd(self, 0.001, x/1000, funct, sdfunct) for x in [2,5,10,15]]

    [scalar_sd(self, 0.003, x/1000, funct, sdfunct) for x in [6,10,15]]

    [scalar_sd(self, 0.005, x/1000, funct, sdfunct) for x in range(10,30,5)]

    [scalar_sd(self, 0.0001, 2/10000, funct, sdfunct) for x in [2,3,6]]

unittest.main(argv=["antares"])
