# Copy of RUN_02_external_data except that this run uses different indices for
# extracting the data from the data file.  
# RUN_02_external_data uses indices 3 and 4, i.e. {{3,13,23},{4,14,24}}
# this run uses indices 1 and 2. i.e. {{1,11,21},{2,12,22}}
# Note that the independent variable also uses index 1.
# Should have the effect of reducing dependent[0] and dependent[1] outputs by 2
var1_config = test.manager.get_config("var1")
var1_config.index_low = 1
var1_config.index_high = 2
exec(open("RUN_02_external_data/input.py").read())
