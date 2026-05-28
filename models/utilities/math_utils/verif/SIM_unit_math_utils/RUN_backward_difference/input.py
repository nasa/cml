exec(open("Log_data/log_math_utils.py").read())
mutil_verif.test_backward_difference = True
log_backward_difference()

# Values based off of cubic equation F(x) = 4x^3 - 3x^2 + 2x + 1
# Uses x values 1, 2, 3, 4, 5
mutil_verif.back_diff_vals[0] = 436
mutil_verif.back_diff_vals[1] = 217
mutil_verif.back_diff_vals[2] = 88
mutil_verif.back_diff_vals[3] = 25
mutil_verif.back_diff_vals[4] = 4
# The backward-difference method is only configured to use a maximum of 5
# values; a 6th value should not be considered. Populate the 6th value
# with a silly number so that if it is considered, it will skew the end
# result and if the end result is as expected, the model successfully ignored
# this value
mutil_verif.back_diff_vals[5] = 999999

trick.stop(6)
