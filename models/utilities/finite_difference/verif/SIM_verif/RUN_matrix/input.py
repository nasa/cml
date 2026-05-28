import sys
import unittest
import finite_difference

exec(open("Modified_data/fake_sys_argv.py").read())
exec(open("Modified_data/functions.py").read())

# use funct(), funct2(), funct3() and their first /
# second derivatives to generate unique values for
# matrix3x3 elements

class Test(unittest.TestCase):

  def compute_and_compare(self, fd, fd_py, x):
    for ii in range(3):
      fd_obj.in_mat3x3[ii][0] = funct (x)*(ii+1)
      fd_obj.in_mat3x3[ii][1] = funct2(x)*(ii+1)
      fd_obj.in_mat3x3[ii][2] = funct3(x)*(ii+1)
    is_good = fd_obj.matrix3x3_first_derivative_compute(fd, x)
    is_good_py, deriv_py = fd_py.compute(fd_obj.in_mat3x3, x)
    self.assertEqual(is_good, is_good_py)
    for ii in range(3):
      for jj in range(3):
        self.assertAlmostEqual(fd_obj.out_mat3x3[ii][jj], deriv_py[ii][jj])
    return fd, fd_py
    
  def test1_first_derivative(self):
    fd = trick.Matrix3x3FirstDerivative()
    fd_py = finite_difference.Matrix3x3FirstDerivative()
    
    for x in [0.0, 0.1, 0.2]:
      fd, fd_py = self.compute_and_compare(fd, fd_py, x)

  def test2_first_derivative(self):
    fd = trick.Matrix3x3FirstDerivative()
    fd_py = finite_difference.Matrix3x3FirstDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.0, 0.01, 0.02]:
        fd, fd_py = self.compute_and_compare(fd, fd_py, x)

  def test3_first_derivative(self):
    fd = trick.Matrix3x3FirstDerivative()
    fd_py = finite_difference.Matrix3x3FirstDerivative()
    
    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.000, 0.001, 0.002]:
        fd, fd_py = self.compute_and_compare(fd, fd_py, x)

  def test4_first_derivative(self):
    fd = trick.Matrix3x3FirstDerivative()
    fd_py = finite_difference.Matrix3x3FirstDerivative()

    def compute_and_compare_test4(fd, fd_py, x):
      fd_obj.in_mat3x3[0][0] = funct3(x)*1
      fd_obj.in_mat3x3[0][1] = funct2(x)*1
      fd_obj.in_mat3x3[0][2] = funct3(x)*2
      fd_obj.in_mat3x3[1][0] = funct2(x)*2
      fd_obj.in_mat3x3[1][1] = funct3(x)*3
      fd_obj.in_mat3x3[1][2] = funct2(x)*3
      fd_obj.in_mat3x3[2][0] = funct3(x)*4
      fd_obj.in_mat3x3[2][1] = funct2(x)*4
      fd_obj.in_mat3x3[2][2] = funct (x)*1
      is_good = fd_obj.matrix3x3_first_derivative_compute(fd, x)
      is_good_py, deriv_py = fd_py.compute(fd_obj.in_mat3x3, x)
      self.assertEqual(is_good, is_good_py)
      for ii in range(3):
        for jj in range(3):
          self.assertAlmostEqual(fd_obj.out_mat3x3[ii][jj], deriv_py[ii][jj])
      return fd, fd_py

    # define even smaller values / timestep to validate that accuracy improves
    for x in [0.0000, 0.0001, 0.0002]:
      fd, fd_py = compute_and_compare_test4(fd, fd_py, x)

unittest.main(argv=["antares"])
