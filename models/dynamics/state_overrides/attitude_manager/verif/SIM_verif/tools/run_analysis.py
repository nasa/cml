"""
Usage:
    ./path/to/ramtares/orion_veras/bin/python.py ./tools/runAnalysis.py
"""

# Define a list of directories (string entries) to load (monte or single run)
from veras import *
from antares.cml.models.dynamics.state_overrides.attitude_manager.verif.SIM_verif.tools.Analysis_vehicle_prescribed_attitude import (
    Analysis_vehicle_prescribed_attitude,
)

# Close any open figures (plt = matplotlib.pyplot)
plt.close('all')

##################################################################################################
# Data Loading
##################################################################################################
# Path to verif sim
sim_path = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

scenarios = []
scenarios.append("RUN_00_baseline")
scenarios.append("RUN_01_subscribe_perturbation_only")
scenarios.append("RUN_02_subscribe_maneuver_only")
scenarios.append("RUN_03_subscribe_both")
scenarios.append("RUN_04_subscribe_perturb_mid_mnvr")
scenarios.append("RUN_05_unsubscribe_perturbation")
scenarios.append("RUN_06_unsubscribe_maneuver")
scenarios.append("RUN_07_enforce_lat_vel")
scenarios.append("RUN_08_smoothing")
scenarios.append("RUN_09_random_rates")
scenarios.append("RUN_10_random_rates_smoothing")
scenarios.append("RUN_11_lvlh_fixed")

path = []
scenario_names = []
for idx in range(len(scenarios)):
    path.append(os.path.join(sim_path, scenarios[idx]))
    scenario_names.append(scenarios[idx])
simdata = SimulationData(path, Name=scenario_names, Quiet=True, LoadTRK=True)

a = Analysis_vehicle_prescribed_attitude(simdata)
a.run()
a.Report.build()
