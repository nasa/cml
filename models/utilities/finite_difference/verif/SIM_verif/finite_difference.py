import os, sys
import numpy as np

def div_protected(num, denom):
    """
    Division with divide by zero protection
    
    Will replace divide-by-zero conditions with zero
    """
    return np.divide(num, denom, out=np.zeros_like(num), where=(denom != 0))

# Methods here are taken straight from 
# antares/cml/models/utilities/finite_difference/docs/ and are meant to act
# as a comparison to the model's output
class ScalarFirstDerivative(object):
    """
    Calculate estimate of first derivative
    """
    def __init__(self):
        # Initialize values
        self.pass_count = 0
        self.value_im1 = 0
        self.value_im2 = 0
        self.time_im1 = 0
        self.time_im2 = 0

    def compute(self, value, time):
        # Values and times are needed from two time steps ago, so pass
        # until compute is called for at least the 3rd time
        if self.pass_count < 2:
            self.pass_count += 1
            is_good = False
            derivative = 0.0
        else:
            is_good = True
            dt1 = time - self.time_im1
            dt2 = time - self.time_im2
            derivative = div_protected((dt1 + dt2),(dt1*dt2)) * value \
                         + div_protected(dt2,(dt1*(dt1 - dt2))) * self.value_im1 \
                         - div_protected(dt1,(dt2*(dt1 - dt2))) * self.value_im2

        # Assign past values as needed
        # Use float() to avoid any reference issues
        self.value_im2 = float(self.value_im1)
        self.value_im1 = float(value)
        self.time_im2 = float(self.time_im1)
        self.time_im1 = float(time)
        return is_good, derivative

class Vector3FirstDerivative(object):
    """
    Create a list of 3 ScalarFirstDerivative() instantiations
    to represent a 3-vector first derivative
    """
    def __init__(self):
        self.scalar_first_derivatives = [ScalarFirstDerivative(),
                                         ScalarFirstDerivative(),
                                         ScalarFirstDerivative()]

    def compute(self, value, time):
        is_good = True
        derivative = [None]*3
        for i in range(3):
            status, derivative[i] = self.scalar_first_derivatives[i].compute(value[i], time)
            is_good = is_good and status
        return is_good, derivative

class Matrix3x3FirstDerivative(object):
    def __init__(self):
        self.scalar_first_derivatives = [[ScalarFirstDerivative(),
                                         ScalarFirstDerivative(),
                                         ScalarFirstDerivative()],
                                         [ScalarFirstDerivative(),
                                          ScalarFirstDerivative(),
                                          ScalarFirstDerivative()],
                                         [ScalarFirstDerivative(),
                                          ScalarFirstDerivative(),
                                          ScalarFirstDerivative()]]

    def compute(self, value, time):
        is_good = True
        derivative = [[None]*3,[None]*3,[None]*3]
        for i in range(3):
            for j in range(3):
                status, derivative[i][j] = self.scalar_first_derivatives[i][j].compute(value[i][j], time)
                is_good = is_good and status
        return is_good, derivative

class ScalarSecondDerivative(object):
    def __init__(self):
        self.pass_count = 0
        self.value_im1 = 0
        self.value_im2 = 0
        self.value_im3 = 0
        self.time_im1 = 0
        self.time_im2 = 0
        self.time_im3 = 0

    def compute(self, value, time):
        is_good = True

        if self.pass_count < 3:
            self.pass_count += 1
            is_good = False
            derivative = 0.0
        else:
            is_good = True
            dt1 = time - self.time_im1
            dt2 = time - self.time_im2
            dt3 = time - self.time_im3

            derivative = 2*(div_protected((dt2 + dt3),(dt1*dt2*(dt3 - dt1))))*value \
                       - 2*(div_protected((dt1 + dt2),(dt2*dt3*(dt3 - dt1))))*value \
                       + 2*(div_protected((dt2 + dt3),(dt1*(dt1 - dt2)*(dt3 - dt1))))*self.value_im1 \
                       + 2*(div_protected((dt1 + dt3),(dt2*(dt1 - dt2)*(dt2 - dt3))))*self.value_im2 \
                       + 2*(div_protected((dt1 + dt2),(dt3*(dt2 - dt3)*(dt3 - dt1))))*self.value_im3

        self.value_im3 = float(self.value_im2)
        self.time_im3 = float(self.time_im2)
        self.value_im2 = float(self.value_im1)
        self.time_im2 = float(self.time_im1)
        self.value_im1 = float(value)
        self.time_im1 = float(time)

        return is_good, derivative

class Vector3SecondDerivative(object):
    def __init__(self):
        self.scalar_second_derivatives = [ScalarSecondDerivative(),
                                         ScalarSecondDerivative(),
                                         ScalarSecondDerivative()]

    def compute(self, value, time):
        is_good = True
        derivative = [None]*3
        for i in range(3):
            status, derivative[i] = self.scalar_second_derivatives[i].compute(value[i],time)
            is_good = is_good and status
        return is_good, derivative
