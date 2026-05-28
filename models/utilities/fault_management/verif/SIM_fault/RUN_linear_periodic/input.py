exec(open('RUN_linear/input.py').read())

# Make the trigger periodic for t=1.0 to 3.5:
test_object.trigger.set_periodic(0.5,1.0)
trick.add_read(3.5,'test_object.trigger.unset_periodic()')
