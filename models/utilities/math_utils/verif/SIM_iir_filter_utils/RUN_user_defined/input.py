exec(open("Log_data/log_data.py").read())

import numpy as np

# Synthetic test wind profile: u(alt) = 10 + 0.5*sin(alt/10000)
alt_min = 0.0
alt_max = 100000.0

# Set filter parameters
test.filter.gain         = 0.95
test.filter.order        = 6
test.filter.wavelength   = 2428.0
test.filter.grid_spacing = 100.0

# Evaluate the profile on a uniform grid before passing to C++
n_pts = int(np.ceil((alt_max - alt_min) / test.filter.grid_spacing)) + 1

alt_uniform = np.linspace(alt_min, alt_max, n_pts)
u_uniform = 10.0 + 0.5 * np.sin(alt_uniform / 10000)

# Set input data (uniform grid)
test.data_length = len(alt_uniform)
for i in range(test.data_length):
    test.x_data[i] = alt_uniform[i]
    test.y_input[i] = u_uniform[i]

log_data()

trick.stop(40)