# This run uses the initial mass as the nominal mass
# It is the baseline run.
exec(open("Log_data/log_data.py").read())
log_test_data(1.0)

exec(open("Modified_data/env_setup.py").read())

trick.stop(11)
