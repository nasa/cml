#This test is to check the safety net provided in the model to catch an error
# in a model extension in which the extension includes commands but provides
# them with no disposition.
exec(open("RUN_1a_simple_sequence/input.py").read())
test.direct_set.error.command_now = True
test.indirect_set.error.command_now = True

print ('\n'
'*************************************************************************\n'
'Error messages (x2):\n'
'Commands have no defined disposition.\n'
'*************************************************************************')
trick.stop(0)
