# This case tests DynamicMassGroup.cc, lines 419-445, successful lookup path.
exec(open("RUN_01_parallel_tanks/input.py").read())

trick.add_read(1.0, '''
if (mass_test.group.is_body_in_group(mass_test.tank_a)):
  print("'tank_a' was found in DynamicMassGroup")
else:
  print("'tank_a' was NOT found in DynamicMassGroup")

if (mass_test.group.is_string_in_group(mass_test.string)):
  print("'string' was found in DynamicMassGroup")
else:
  print("'string' was NOT found in DynamicMassGroup")
''')

trick.stop(1)
