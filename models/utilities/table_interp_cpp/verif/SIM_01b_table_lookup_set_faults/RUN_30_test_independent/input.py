# Tests several modes of loading invalid data.  
# These use STL-contrainers, so the tests are embedded in code rather than
# going through SWIG.
test.load_invalid_independent_data()

print("*** TEST update an uninitialized TIV  ***")
test.tiv_1.update()
trick.stop(0)
