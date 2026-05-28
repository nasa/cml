# Try a file with one line too long, this should work.
print("*** process data containing invalid independent data ***")
test.process_corrupt_data_reversed()

print("*** initialize without a table ***")
test.initialize()

# Try running with corrupt data on the long-side.  This should be fine.
test.process_corrupt_data_long()

print("*** re-process the data prior to initialization. ***")
test.process_corrupt_data_long()

# Now initialize
test.initialize()

print("*** Re-process data post-initialization ***")
test.process_internal_data()

trick.stop(0)
