#!/usr/bin/env python3
# This script provide independent verification of aspects of
# runs in the state_initialize model's SIM_state_init unit sim
# This was used as part of EM1 iV&V effort

import inspect, sys, os, pdb
thisDir = os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))
sys.path.append(os.path.abspath(os.path.join(thisDir,"../../../../state_descriptors/range/verif/")))
from SIM_unit_tests import verif
import numpy as np
import math
from argparse import ArgumentParser

def getArgs():
    """
    """
    parser = ArgumentParser(description='Provides independent verification of aspects of runs in the state_initialize model\'s SIM_state_init unit sim.  Not intended to be run with any arguments. Output is intended to be manually compared to the output of various runs in the verif sim.')
    myArgs = parser.parse_args()

    return myArgs

def main():
    """
    """
    myArgs = getArgs()

    print("RUN_pos_NED_Geod reference point to ECEF conversion:")
    ecef = verif.geo_to_ecef(math.pi/4, math.pi/2, 100000)  # Only position different from RUN_test
    print(ecef)
    print("RUN_pos_NED_Geod ECEF position (add 10000 m in Down dir)")
    pos = [0, -np.cos(np.pi/4)*10000, -np.sin(np.pi/4)*10000]
    print(ecef + pos)
    print("RUN_vel_inrtlSpeed position to spherical alt/lat/long conversion:")
    print(verif.cart_to_sphere(6800000, 0, 0))    # Only position different from RUN_test

if __name__ == "__main__":
    sys.exit(main())
