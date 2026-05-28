# Tests two features:
# process_reduced_file reads 2 lines from file_2.txt,
# file_2.txt has irregular line lengths
# it also has 3 lines.
# should detect irregular line lengths and that reading stops before the end of
# the file.
print("*** TEST detect irregular line length and file continuation beyond read")
test.process_reduced_file()

trick.stop(0)
