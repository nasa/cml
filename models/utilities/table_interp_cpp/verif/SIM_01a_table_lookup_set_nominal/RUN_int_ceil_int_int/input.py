exec(open("RUN_interp_all/input.py").read())

# This test is used to perform a few of the different options for calling
# associate_table_and_independent at table_lookup_set.cc:134 & 157 & 170 & 181. This
# test is specifically using the "Ceil" lookup method in order to make sure the
# model works as intended with this lookup type
testing_model.test_on_the_fly.lookup_method[1] = trick.TableIndependentVariable.Ceil
testing_model.test_so_defined.lookup_method[1] = trick.TableIndependentVariable.Ceil
