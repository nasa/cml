# Purpose:
# - test the detection of an invalid rocket-height at initialization
# - test the reset of the rocket height to an invalid number post-initialization
exec(open("RUN_00_baseline/input.py").read())
ts_so.ts.params.RocketHeight = 0.0

print('\n'
'*****************************************************************************\n'
' Invalid rocket-height at initialization:\n'
' Follow-up that activation is pending initialization\n'
'*****************************************************************************')

trick.add_read(1,"""
print('\\n'
'*****************************************************************************\\n'
' Invalid rocket-height post-initialization:\\n'
'*****************************************************************************')
ts_so.ts.params.RocketHeight = 10.0
ts_so.ts.initialize(300)
ts_so.ts.params.RocketHeight = 0.0
""")

trick.stop(1)
