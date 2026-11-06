import sys
import unittest
import math

exec(open("Modified_data/fake_sys_argv.py").read())
exec(open("Modified_data/functions.py").read())

class Test(unittest.TestCase):

  def test_first_derivative(self):
    fd = trick.ScalarFirstDerivative()

    is_good, deriv = fd.compute(funct(0.0), 0.0)

    is_good, deriv = fd.compute(funct(0.1), 0.1)

    is_good, deriv = fd.compute(funct(0.2), 0.2)

    # jump time tremendously to trigger overflow...
    # this is the lowest exponential value needed to trigger
    # overflow
    is_good, deriv = fd.compute(funct(0.3), 1.0e16)

unittest.main(argv=["antares"])
