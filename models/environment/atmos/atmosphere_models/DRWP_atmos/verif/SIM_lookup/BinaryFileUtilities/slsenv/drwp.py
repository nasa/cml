import numpy as np
import struct

def load_binary(slsfile,use_w=False):
   with open(slsfile,'rb') as bf:
      # Binary file header info:
      nProfiles, = np.fromfile(bf,'<i4',1)
      nBPoints,  = np.fromfile(bf,'<i4',1)
      alt        = np.fromfile(bf,'<f4',nBPoints)
      Profiles   = np.fromfile(bf,'<i4',nProfiles)
      # Define dtype for repetative read
      if use_w:
         dt = np.dtype([('profile','<i4', 1       ),
                        ('u'      ,'<f4', nBPoints),
                        ('v'      ,'<f4', nBPoints),
                        ('w'      ,'<f4', nBPoints),
                        ('temp'   ,'<f4', nBPoints),
                        ('rho'    ,'<f4', nBPoints),
                        ('press'  ,'<f4', nBPoints)])
      else:
         dt = np.dtype([('profile','<i4', 1       ),
                        ('u'      ,'<f4', nBPoints),
                        ('v'      ,'<f4', nBPoints),
                        ('temp'   ,'<f4', nBPoints),
                        ('rho'    ,'<f4', nBPoints),
                        ('press'  ,'<f4', nBPoints)])

      # Create dictionary of profiles
      # NOTE: Python2.6 doesn't support dictionary comprehension as Python2.7
      # TODO: Make check for Python version
      # data = {P:np.squeeze(np.fromfile(bf,dt,1)) for P in Profiles}
      data = dict( (P, np.squeeze(np.fromfile(bf,dt,1))) for P in Profiles )
      # Add altitude to dictionary
      data['alt'] = alt
      return data

def write_binary(w,outfile="DRWP.bin",use_w=False):
   '''
   Input "w" is the output from load_binary.
   This function is intended to build data from the load_binary() function, and 
   manipulate it for testing purposes.
   '''
   precision = 'f'
   wind_set = list(w.keys())
   wind_set.remove('alt')
   nBPoints = len(w['alt'])

   with open(outfile,'wb') as bf:
      # 4            int      Number of wind profiles stored in binary file (nProfiles)   2000
      bf.write(struct.pack('I',len(wind_set)))
      # 4            int      Number of alt breakpoints   727
      bf.write(struct.pack('I',nBPoints))
      # 4*nAlts      float    Altitude breakpoints (lu_alt_array) 0.0, 50.0, 100.0,...
      bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w['alt'])))
      # 4*nProfiles  int      Wind profile numbers 2, 4, 6, 8,...
      print(wind_set)
      bf.write(struct.pack('{0}I'.format(len(wind_set)),*wind_set))

      for WINDNUM in wind_set:
         print(WINDNUM)
         # 4            int      Wind number of following data
         bf.write(struct.pack('I',WINDNUM))
         # 4*nAlts      float    U wind velocity for wind profile N (lu_u_array)
         bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['u'])))
         # 4*nAlts      float    V wind velocity for wind profile N (lu_v_array)
         bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['v'])))

         if use_w:
            # 4*nAlts      float    W wind velocity for wind profile N (lu_w_array)
            bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['w'])))

         # 4*nAlts      float    Temperature for wind profile N (lu_temp_array)
         bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['temp'])))
         # 4*nAlts      float    Density for wind profile N (lu_rho_array)
         bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['rho'])))
         # 4*nAlts      float    Pressure for wind profile N (lu_press_array)
         bf.write(struct.pack('{0}{1}'.format(nBPoints,precision),*(w[WINDNUM]['press'])))