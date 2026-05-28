# Tests a corrupt data set that contains one line with insufficient data for
# the specified indices.
print("\n*** Test effect of trying to process a file with one line too small *****")
test.process_corrupt_data_short()

trick.stop(0)
