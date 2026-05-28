#Test the case where there are more than 1 independent variable but one of
#those independent variables is a trivial single data point while the other is
#interpolated.
exec(open("Log_data/log_data.py").read())

test.use_config = True

trick.stop(4)
