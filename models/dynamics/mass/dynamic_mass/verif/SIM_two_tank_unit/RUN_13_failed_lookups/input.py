# This case tests DynamicMassGroup.cc, lines 419-445, failed lookup path.
exec(open("RUN_01_parallel_tanks/input.py").read())

trick.add_read(1.0, '''
if (mass_test.group.is_body_in_group(mass_test.dummy_body) == False):
  print("'dummy_body' was not found AS EXPECTED in DynamicMassGroup")
else:
  print("'dummy_body' was UNEXPECTELY found in DynamicMassGroup")

if (mass_test.group.is_string_in_group(mass_test.dummy_string) == False):
  print("'dummy_string' was not found AS EXPECTED in DynamicMassGroup")
else:
  print("'dummy_string' was UNEXPECTELY found in DynamicMassGroup")
''')

trick.stop(1)
