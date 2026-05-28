# Check the filename entered actually exists.
# Use "Hello" as an arbitrary meaningless name
print("\n*** Test effect of trying to process a file that doesn't exist *****")
test.manager.process_data("data/Hello.txt")

# Check the name entered to be looked up actually exists.
# Use "Hello" as an arbitrary meaningless name
print("\n******** Test effect of fake configuration name *****")
test.manager.get_config("Hello")

# Note - model already disabled by invalid filename spec.
#        so don't need to actively disable it to test this.
print("\n******** Test effect of disabling model *****")
test.manager.process_data( "data/text_file.txt")

trick.stop(0)
