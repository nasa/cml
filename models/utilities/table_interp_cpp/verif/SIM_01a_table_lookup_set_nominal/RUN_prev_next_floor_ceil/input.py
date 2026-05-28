exec(open("RUN_interp_all/input.py").read())

# This test is used to perform all of the different options for calling
# associate_table_and_independent at table_lookup_set.cc. This
# test is using 4/6 of the lookup methods in order to make sure the
# model works as intended when using all lookup methods at once to ensure that they
# can be used separately but together at the same time.
testing_model.test_on_the_fly.lookup_method[0] = trick.TableIndependentVariable.Prev
testing_model.test_so_defined.lookup_method[0] = trick.TableIndependentVariable.Prev
testing_model.test_on_the_fly.lookup_method[1] = trick.TableIndependentVariable.Next
testing_model.test_so_defined.lookup_method[1] = trick.TableIndependentVariable.Next
testing_model.test_on_the_fly.lookup_method[2] = trick.TableIndependentVariable.Floor
testing_model.test_so_defined.lookup_method[2] = trick.TableIndependentVariable.Floor
testing_model.test_on_the_fly.lookup_method[3] = trick.TableIndependentVariable.Ceil
testing_model.test_so_defined.lookup_method[3] = trick.TableIndependentVariable.Ceil
