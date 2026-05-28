exec(open("RUN_00_baseline/input.py").read())
print('\n'
'*****************************************************************************\n'
' Warning - zero-period error detected.\n'
'*****************************************************************************')
ts_so.ts.params.delta_T1_max = 0
ts_so.ts.params.delta_T1_min = 0
trick.stop(1)
