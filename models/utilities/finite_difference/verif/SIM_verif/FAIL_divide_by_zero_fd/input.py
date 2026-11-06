import sys
import unittest
import math

exec(open("Modified_data/fake_sys_argv.py").read())

class Test(unittest.TestCase):

    def test_scalar_first_derivative(self):
        fd = trick.ScalarFirstDerivative()

        is_good, deriv = fd.compute(math.sin(0.0), 0.0)

        is_good, deriv = fd.compute(math.sin(0.1), 0.1)

        is_good, deriv = fd.compute(math.sin(0.2), 0.2)

        # test duplicate value and time to trip up fd.compute() 
        is_good, deriv = fd.compute(math.sin(0.2), 0.2)

unittest.main(argv=["antares"])
