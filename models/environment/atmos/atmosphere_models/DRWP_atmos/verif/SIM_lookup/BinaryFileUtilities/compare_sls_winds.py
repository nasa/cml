#!/usr/bin/env python3

# This here python script was built to double check the loading/interpolation of
# DRWP binary file done by the DRWP model.  The 'model' doesn't really do
# anything of significance other than load and interpolate data.

import numpy as np
import sys,os
import matplotlib.pyplot as plt

HERE = os.path.dirname(os.path.realpath(__file__))
sys.path.append(os.path.join(HERE,'slsenv'))

import slsenv

# Load RUN_unit file:
with open(os.path.join(HERE,'../RUN_01_preload_5_nospec','log_lu_winds.csv'),'r') as f:
   V = f.readline().strip().split(',')
   D = np.loadtxt(f,delimiter=',')
#   print(V)
#   print(D)
   UT_data = dict((variable.split()[0],data) for variable,data in zip(V,D.T))
   print(UT_data)

# Set wind data.  'Winter' season and wind number 1771
wind_file = os.path.join( HERE, '../Binaries/DRWP_no_w_comp.bin')
wind_number = int(UT_data['luwinds.lookup_table_winds.wind_number'][0])
w_all = slsenv.load_binary(wind_file)
w_alt = w_all['alt']
w     = w_all[wind_number]

#print (w)

# Interpolate via python
interp_vars = ['u', 'v', 'temp', 'rho', 'press']
interp_data = dict( (IV,np.interp(UT_data['luwinds.altitude'],w_alt,w[IV])) for IV in interp_vars )

# Determine deltas
du   = UT_data['luwinds.lookup_table_winds.u'] - interp_data['u']
dv   = UT_data['luwinds.lookup_table_winds.v'] - interp_data['v']
dP   = UT_data['luwinds.lookup_table_winds.P'] - interp_data['press']
dT   = UT_data['luwinds.lookup_table_winds.T'] - interp_data['temp']
drho = UT_data['luwinds.lookup_table_winds.rho'] - interp_data['rho']

print("**************** du ******************\n", du)
print("**************** dv ******************\n", dv)
print("**************** dP ******************\n", dP)
print("**************** dT ******************\n", dT)
print("**************** drho ****************\n", drho)

plt.figure(figsize=(10,8))
plt.suptitle('U')
plt.subplot(121)
plt.plot(UT_data['luwinds.lookup_table_winds.u'],UT_data['luwinds.altitude'],interp_data['u'],UT_data['luwinds.altitude'])
plt.title('Coplot')
plt.legend(('Unit Sim','PYTHON Interp'))
plt.subplot(122,sharey=plt.gca())
plt.title('DELTA')
plt.plot(du,UT_data['luwinds.altitude'])

plt.figure(figsize=(10,8))
plt.suptitle('V')
plt.subplot(121)
plt.plot(UT_data['luwinds.lookup_table_winds.v'],UT_data['luwinds.altitude'],interp_data['v'],UT_data['luwinds.altitude'])
plt.title('Coplot')
plt.legend(('Unit Sim','PYTHON Interp'))
plt.subplot(122,sharey=plt.gca())
plt.title('DELTA')
plt.plot(dv,UT_data['luwinds.altitude'])

plt.figure(figsize=(10,8))
plt.suptitle('Pressure')
plt.subplot(121)
plt.plot(UT_data['luwinds.lookup_table_winds.P'],UT_data['luwinds.altitude'],interp_data['press'],UT_data['luwinds.altitude'])
plt.title('Coplot')
plt.legend(('Unit Sim','PYTHON Interp'))
plt.subplot(122,sharey=plt.gca())
plt.title('DELTA')
plt.plot(dP,UT_data['luwinds.altitude'])

plt.figure(figsize=(10,8))
plt.suptitle('Temperature')
plt.subplot(121)
plt.plot(UT_data['luwinds.lookup_table_winds.rho'],UT_data['luwinds.altitude'],interp_data['rho'],UT_data['luwinds.altitude'])
plt.title('Coplot')
plt.legend(('Unit Sim','PYTHON Interp'))
plt.subplot(122,sharey=plt.gca())
plt.title('DELTA')
plt.plot(dT,UT_data['luwinds.altitude'])

plt.figure(figsize=(10,8))
plt.suptitle('Density')
plt.subplot(121)
plt.plot(UT_data['luwinds.lookup_table_winds.T'],UT_data['luwinds.altitude'],interp_data['temp'],UT_data['luwinds.altitude'])
plt.title('Coplot')
plt.legend(('Unit Sim','PYTHON Interp'))
plt.subplot(122,sharey=plt.gca())
plt.title('DELTA')
plt.plot(drho,UT_data['luwinds.altitude'])

plt.show()
