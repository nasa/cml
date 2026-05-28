# This test checks the detection of a NULL output at construction time.
print("***FAIL: Try making a table with a NULL dependent variable ***")
new_table = trick.GenericMultiInputTable(None,1)
trick.stop(0)
