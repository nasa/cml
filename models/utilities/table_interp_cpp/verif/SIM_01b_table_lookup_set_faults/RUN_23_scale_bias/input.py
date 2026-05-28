# Trying to scale and bias before loading data
test.generic_table.scale_data(2.0)
test.generic_table.bias_data(1.0)

exec(open("Modified_data/common_input.py").read())

# Scaling and biasing with a start index greater than the stop index
test.generic_table.scale_data(2.0, 3, 2)
test.generic_table.bias_data(1.0, 3, 2)

# Trying to scale and bias with an invalid stop index
test.generic_table.scale_data(2.0, 9999)
test.generic_table.bias_data(1.0, 3, 9999)
