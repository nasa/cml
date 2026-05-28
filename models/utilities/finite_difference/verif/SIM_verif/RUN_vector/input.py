import sys
import unittest
import finite_difference

exec(open("Modified_data/fake_sys_argv.py").read())
exec(open("Modified_data/functions.py").read())

# use funct(), funct2(), funct3() and their first /
# second derivatives to generate unique values for
# vector3 elements

class Test(unittest.TestCase):

  def compute_and_compare(self, fd, fd_py, x, derivative='first'):
    fd_obj.in_vec3[0] = funct (x)
    fd_obj.in_vec3[1] = funct2(x)
    fd_obj.in_vec3[2] = funct3(x)
    if derivative == 'first':
      is_good = fd_obj.vector3_first_derivative_compute(fd, x)
    else:
      is_good = fd_obj.vector3_second_derivative_compute(fd, x)
    is_good_py, deriv_py = fd_py.compute(fd_obj.in_vec3, x)
    self.assertEqual(is_good, is_good_py)
    for ii in range(3):
      self.assertAlmostEqual(fd_obj.out_vec3[ii], deriv_py[ii])

    return fd, fd_py

  def test1_first_derivative(self):
    fd = trick.Vector3FirstDerivative()
    fd_py = finite_difference.Vector3FirstDerivative()
    
    for x in [0.0, 0.1, 0.2]:
      fd, fd_py = self.compute_and_compare(fd, fd_py, x)

  def test2_first_derivative(self):
    fd = trick.Vector3FirstDerivative()
    fd_py = finite_difference.Vector3FirstDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.00, 0.01, 0.02]:
      fd, fd_py = self.compute_and_compare(fd, fd_py, x)
  
  def test3_first_derivative(self):
    fd = trick.Vector3FirstDerivative()
    fd_py = finite_difference.Vector3FirstDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.000, 0.001, 0.002]:
      fd, fd_py = self.compute_and_compare(fd, fd_py, x)
  
  def test4_first_derivative(self):
    fd = trick.Vector3FirstDerivative()
    fd_py = finite_difference.Vector3FirstDerivative()

    def compute_and_compare_test4(fd, fd_py, x):
      fd_obj.in_vec3[0] = funct2(x)
      fd_obj.in_vec3[1] = funct3(x)
      fd_obj.in_vec3[2] = funct2(x)*2
      is_good = fd_obj.vector3_first_derivative_compute(fd, x)
      is_good_py, deriv_py = fd_py.compute(fd_obj.in_vec3, x)
      self.assertEqual(is_good, is_good_py)
      for ii in range(3):
        self.assertEqual(fd_obj.out_vec3[ii], deriv_py[ii])

      return fd, fd_py
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.0000, 0.0001, 0.0002]:
        fd, fd_py = compute_and_compare_test4(fd, fd_py, x)

  def test5_second_derivative(self):
    sd = trick.Vector3SecondDerivative()
    sd_py = finite_difference.Vector3SecondDerivative()

    for x in [0.0, 0.1, 0.2, 0.3]:
      sd, sd_py = self.compute_and_compare(sd, sd_py, x, derivative='second')

  def test6_second_derivative(self):
    sd = trick.Vector3SecondDerivative()
    sd_py = finite_difference.Vector3SecondDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.00, 0.01, 0.02, 0.03]:
      sd, sd_py = self.compute_and_compare(sd, sd_py, x, derivative='second')

  def test7_second_derivative(self):
    sd = trick.Vector3SecondDerivative()
    sd_py = finite_difference.Vector3SecondDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.000, 0.001, 0.002, 0.003]:
      sd, sd_py = self.compute_and_compare(sd, sd_py, x, derivative='second')

  def test8_second_derivative(self):
    sd = trick.Vector3SecondDerivative()
    sd_py = finite_difference.Vector3SecondDerivative()

    def compute_and_compare_test8(sd, sd_py, x):
      fd_obj.in_vec3[0] = funct3(x)
      fd_obj.in_vec3[1] = funct2(x)
      fd_obj.in_vec3[2] = funct3(x)*2
      is_good = fd_obj.vector3_second_derivative_compute(sd, x)
      is_good_py, deriv_py = sd_py.compute(fd_obj.in_vec3, x)
      self.assertEqual(is_good, is_good_py)
      for ii in range(3):
        self.assertEqual(fd_obj.out_vec3[ii], deriv_py[ii])

      return sd, sd_py

    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.0000, 0.0001, 0.0002, 0.0003]:
      sd, sd_py = compute_and_compare_test8(sd, sd_py, x)

unittest.main(argv=["antares"])
