import sys
import unittest
import math

exec(open("Modified_data/fake_sys_argv.py").read())
exec(open("Modified_data/functions.py").read())

class Test(unittest.TestCase):

  def test_second_derivative(self):
    sd = trick.ScalarSecondDerivative()

    is_good, deriv = sd.compute(funct(0.0), 0.0)

    is_good, deriv = sd.compute(funct(0.1), 0.1)

    is_good, deriv = sd.compute(funct(0.2), 0.2)

    is_good, deriv = sd.compute(funct(0.3), 0.3)

    # jump time tremendously to trigger overflow...
    # this is the lowest exponential value needed to trigger
    # overflow
    is_good, deriv = sd.compute(funct(0.4), 1.0e15)

unittest.main(argv=["antares"])
