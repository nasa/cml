env.de4xx.set_model_number(421)
exec(open("Log_data/log_data.py").read())
exec(open("Modified_data/env_setup.py").read())

def common_input (prop_time):
  test.prop_time = prop_time
  if prop_time > 0.0:
    log_test_data(prop_time)

  # Adjust the env so that planet aligns at end of integration
  # with respect to RUN_baseline
  jeod_time.time_tai.initializing_value = prop_time

  test.state_init.propagation_time = prop_time
  test.state_init.time_step = 1.0
  trick.stop(1)


def common_input_rev (prop_time):
  common_input(prop_time)
  test.prop_time = -prop_time
  test.state_init.propagation_time = -prop_time
  jeod_time.time_tai.initializing_value = 0
  trick.stop(2 + prop_time)
