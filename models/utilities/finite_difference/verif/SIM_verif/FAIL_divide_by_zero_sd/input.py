import sys
import unittest
import math

exec(open("Modified_data/fake_sys_argv.py").read())

class Test(unittest.TestCase):

    def test_scalar_second_derivative(self):
        sd = trick.ScalarSecondDerivative()

        is_good, deriv = sd.compute(math.sin(0.0), 0.0)

        is_good, deriv = sd.compute(math.sin(0.1), 0.1)

        is_good, deriv = sd.compute(math.sin(0.2), 0.2)

        is_good, deriv = sd.compute(math.sin(0.3), 0.3)

        # test duplicate value and time to trip up sd.compute() 
        is_good, deriv = sd.compute(math.sin(0.3), 0.3)

unittest.main(argv=["antares"])
