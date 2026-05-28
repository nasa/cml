#!/usr/bin/env python3

import numpy as np

class State():
   '''
   Class to hold MassBody state data
   '''
   def __init__(self):
      self.P = 0.0
      self.V = 0.0
      self.A = 0.0

class LogMB():
   '''
   Dumb logging class.  No idea how to do this ,so I just made it up.
   '''
   def __init__(self):
      self.P = []
      self.V = []
      self.A = []
      self.dt = []
      self.force = []
   def log(self,MBody,dt):
      self.P.append(MBody.state.P)
      self.V.append(MBody.state.V)
      self.A.append(MBody.state.A)
      self.force.append(MBody.force)
      self.dt.append(dt)
      self.t = np.cumsum(self.dt) - np.array(self.dt)

class MassBody():
   '''
   Class to describe a point mass
   '''
   def __init__(self,mass=1.0):
      self.mass  = mass
      self.state = State()
      self.force = 0.0
      self.log   = LogMB()

   def update_state(self,dt):
      a = self.force / self.mass
      self.state.A = a
      self.log.log(self,dt)
      v = self.state.V + a*dt
      x = self.state.P + self.state.V*dt + 0.5*a*dt*dt
      self.state.P = x
      self.state.V = v

class Spring():
   '''
   Dumb spring force computation
   '''
   def __init__(self,k=1.0,disp=0.0,stroke=0.0,active=True):
      self.k      = k
      self.disp   = disp
      self.stroke = stroke
      self.F      = 0.0
      self.active = active

   def activate(self):
      self.active = True

   def compute_force(self):
      if (np.abs(self.disp) > self.stroke) or not self.active :
         self.F = 0.0
      else:
         self.F = (self.stroke - self.disp) * self.k


if __name__ == '__main__':
   import matplotlib.pyplot as plt

   def load_trk_csv(FILE):
      with open(FILE,'r') as f:
         header = f.readline().strip().split(',')
         V,units = zip(*[var.split() for var in header])
         data   = np.loadtxt(f,delimiter=',')
         D = dict( (dVar,R) for dVar,R in zip(V,data.T) ) 
         # D = {dVar:R for dVar,R in zip(V,data.T)}
         return D
   
   trk   = load_trk_csv('RUN_spring_single_verif/log_test_data_base.csv')   # define mass props
   trk2  = load_trk_csv('RUN_spring_aligned_verif/log_test_data.csv')   # define mass props
   trk3  = load_trk_csv('RUN_spring_aligned_diff_constant_verif/log_test_data.csv')   # define mass props
   
   CM = MassBody(1.0)
   CM.state.P = 1.0
   SM = MassBody(1.0)
   SM.state.P = 1.0
   spr = Spring(k=20,stroke=.05,active=False)

   # Loop
   t = 0.0
   dt = 0.01
   while t < 2.0:
      if t >=1.0:
         spr.activate()
      spr.disp = CM.state.P - SM.state.P
      spr.compute_force()
      CM.force =  spr.F
      SM.force = -spr.F
      CM.update_state(dt)
      SM.update_state(dt)
      t += dt

   # PLOT
   plt.rc('lines',linewidth=2)
   plt.rc('axes',grid=True)
   plt.rc('grid',linestyle=':')
   plt.figure(figsize=(8,11))
   plt.suptitle('Independent Model of Simple Spring Comparison',fontsize=16,fontweight='bold')
   plt.subplot(311)
   plt.plot(CM.log.t,CM.log.force,label='AB: spr_calc')
   plt.plot(SM.log.t,SM.log.force,label='RB: spr_calc')
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.detach.springs.spring_array[0].axial_force'],'r--',label='AB: RUN_ssv')
   plt.ylabel('Force (N)')
   plt.legend(loc='best')

   plt.subplot(312,sharex=plt.gca())
   plt.plot(CM.log.t,CM.log.V,label='AB: spr_calc')
   plt.plot(SM.log.t,SM.log.V,label='RB: spr_calc')
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.body.composite_body.state.trans.velocity[0]'],'r--',label='AB: RUN_ssv')
   plt.ylabel('Velocity (m/s)')
   plt.legend(loc='best')

   plt.subplot(313,sharex=plt.gca())
   plt.plot(CM.log.t,CM.log.P,label='AB: spr_calc')
   plt.plot(SM.log.t,SM.log.P,label='RB: spr_calc')
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.body.composite_body.state.trans.position[0]'],'r--',label='AB: RUN_ssv')
   plt.ylabel('Position (m)')
   plt.legend(loc='best')

   plt.show()

   plt.rc('lines',linewidth=2)
   plt.rc('axes',grid=True)
   plt.rc('grid',linestyle=':')
   plt.figure(figsize=(8,11))
   plt.suptitle('Independent Model of Simple Spring Comparison',fontsize=16,fontweight='bold')
   plt.subplot(311)
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.detach.springs.spring_array[0].axial_force'],'r',label='AB: RUN_ssv')
   plt.plot(trk2['sys.exec.out.time'],trk2['veh_action.detach.springs.spring_array[0].axial_force'],'b--',label='AB: RUN_sav')
   plt.plot(trk3['sys.exec.out.time'],trk3['veh_action.detach.springs.spring_array[0].axial_force'],'g--',label='AB: RUN_sadcv')
   plt.ylabel('Force (N)')
   plt.yticks(np.arange(-1,1,step=0.5))
   plt.legend(loc='best')

   plt.subplot(312,sharex=plt.gca())
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.body.composite_body.state.trans.velocity[0]'],'r',label='AB: RUN_ssv')
   plt.plot(trk2['sys.exec.out.time'],trk2['veh_action.body.composite_body.state.trans.velocity[0]'],'b--',label='AB: RUN_sav')
   plt.plot(trk3['sys.exec.out.time'],trk3['veh_action.body.composite_body.state.trans.velocity[0]'],'g--',label='AB: RUN_sadcv')
   plt.ylabel('Velocity[0] (m/s)')
   plt.yticks(np.arange(-0.20,0.20,step=0.05))
   plt.legend(loc='best')

   plt.subplot(313,sharex=plt.gca())
   plt.plot(trk['sys.exec.out.time'],trk['veh_action.body.composite_body.state.trans.position[0]'],'r',label='AB: RUN_ssv')
   plt.plot(trk2['sys.exec.out.time'],trk2['veh_action.body.composite_body.state.trans.position[0]'],'b--',label='AB: RUN_sav')
   plt.plot(trk3['sys.exec.out.time'],trk3['veh_action.body.composite_body.state.trans.position[0]'],'g--',label='AB: RUN_sadcv')
   plt.ylabel('Position[0] (m)')
   plt.yticks(np.arange(0.85,1.15,step=0.05))
   plt.legend(loc='best')
 
   plt.show()

   plt.rc('lines',linewidth=2)
   plt.rc('axes',grid=True)
   plt.rc('grid',linestyle=':')
   plt.figure(figsize=(8,11))
   plt.suptitle('Independent Model of Simple Spring Comparison',fontsize=16,fontweight='bold')
   plt.subplot(211)
   plt.plot(trk2['sys.exec.out.time'],trk2['veh_action.detach.springs.spring_array[0].axial_force'],'b--',label='AB: RUN_sav')
   plt.plot(trk3['sys.exec.out.time'],trk3['veh_action.detach.springs.spring_array[0].axial_force'],'g--',label='AB: RUN_sadcv')
   plt.ylabel('Force (N)')
   plt.yticks(np.arange(-1,1,step=0.5))
   plt.legend(loc='best')

   plt.subplot(212,sharex=plt.gca())
   plt.plot(trk2['sys.exec.out.time'],trk2['veh_action.detach.springs.spring_array[1].axial_force'],'b--',label='AB: RUN_sav')
   plt.plot(trk3['sys.exec.out.time'],trk3['veh_action.detach.springs.spring_array[1].axial_force'],'g--',label='AB: RUN_sadcv')
   plt.ylabel('Force (N)')
   plt.yticks(np.arange(-1,1,step=0.5))
   plt.legend(loc='best')

   plt.show()
