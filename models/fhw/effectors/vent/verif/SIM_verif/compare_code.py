angVel0 = 0.0
prevAngVel0 = 0.0
angVel1 = 0.0
prevAngVel1 = 0.0
angVel2 = 0.0
prevAngVel2 = 0.0
force0 = 0.0
prevForce0 = 0.0
force1 = 0.0
prevForce1 = 0.0
force2 = 0.0
prevForce2 = 0.0
tank0Mass = 0.0
prevTank0Mass = 0.0
tank1Mass = 0.0
prevTank1Mass = 0.0
torque0 = 0.0
prevTorque0 = 0.0
torque1 = 0.0
prevTorque1 = 0.0
torque2 = 0.0
prevTorque2 = 0.0
vel0 = 0.0
prevVel0 = 0.0
vel1 = 0.0
prevVel1 = 0.0
vel2 = 0.0
prevVel2 = 0.0
initializedTank0Mass = 0.0
initializedTank1Mass = 0.0
vent0MassChange = 0.0
vent1MassChange = 0.0
vent2MassChange = 0.0
vent3MassChange = 0.0

def extractAngVel():
  global angVel0, prevAngVel0, angVel1, prevAngVel1, angVel2, prevAngVel2
  prevAngVel0 = angVel0
  angVel0 = float(ventset_obj.root_body.composite_body.state.rot.ang_vel_this[0])
  print("angVel0      : "+str(angVel0)+" N*m*s")
  prevAngVel1 = angVel1
  angVel1 = float(ventset_obj.root_body.composite_body.state.rot.ang_vel_this[1])
  print("angVel1      : "+str(angVel1)+" N*m*s")
  prevAngVel2 = angVel2
  angVel2 = float(ventset_obj.root_body.composite_body.state.rot.ang_vel_this[2])
  print("angVel2      : "+str(angVel2)+" N*m*s")

def angVel0Changed():
  global angVel0, prevAngVel0
  return (angVel0 != prevAngVel0)

def angVel1Changed():
  global angVel1, prevAngVel1
  return (angVel1 != prevAngVel1)

def angVel2Changed():
  global angVel2, prevAngVel2
  return (angVel2 != prevAngVel2)


def extractForce():
  global force0, prevForce0, force1, prevForce1, force2, prevForce2
  prevForce0 = force0
  force0 = float(ventset_obj.set_2_tanks.force[0])
  print("force0       : "+str(force0)+" N")
  prevForce1 = force1
  force1 = float(ventset_obj.set_2_tanks.force[1])
  print("force1       : "+str(force1)+" N")
  prevForce2 = force2
  force2 = float(ventset_obj.set_2_tanks.force[2])
  print("force2       : "+str(force2)+" N")

def force0Changed():
  global force0, prevForce0
  return (force0 != prevForce0)

def force1Changed():
  global force1, prevForce1
  return (force1 != prevForce1)

def force2Changed():
  global force2, prevForce2
  return (force2 != prevForce2)


def extractMass():
  global tank0Mass, prevTank0Mass, tank1Mass, prevTank1Mass
  prevTank0Mass = tank0Mass
  tank0Mass = float(ventset_obj.tank[0].dynamic_properties.consumable_mass)
  print("tank[0] mass : "+str(tank0Mass)+" kg")
  prevTank1Mass = tank1Mass
  tank1Mass = float(ventset_obj.tank[1].dynamic_properties.consumable_mass)
  print("tank[1] mass : "+str(tank1Mass)+" kg")

def mass0Changed():
  global tank0Mass, prevTank0Mass
  return (tank0Mass != prevTank0Mass)

def mass1Changed():
  global tank1Mass, prevTank1Mass
  return (tank1Mass != prevTank1Mass)


def extractTorque():
  global torque0, prevTorque0, torque1, prevTorque1, torque2, prevTorque2
  prevTorque0 = torque0
  torque0 = float(ventset_obj.set_2_tanks.torque[0])
  print("torque0      : "+str(torque0)+" N*m")
  prevTorque1 = torque1
  torque1 = float(ventset_obj.set_2_tanks.torque[1])
  print("torque1      : "+str(torque1)+" N*m")
  prevTorque2 = torque2
  torque2 = float(ventset_obj.set_2_tanks.torque[2])
  print("torque2      : "+str(torque2)+" N*m")

def torque0Changed():
  global torque0, prevTorque0
  return (torque0 != prevTorque0)

def torque1Changed():
  global torque1, prevTorque1
  return (torque1 != prevTorque1)

def torque2Changed():
  global torque2, prevTorque2
  return (torque2 != prevTorque2)


def extractVelocity():
  global vel0, prevVel0, vel1, prevVel1, vel2, prevVel2
  prevVel0 = vel0
  vel0 = float(ventset_obj.root_body.composite_body.state.trans.velocity[0])
  print("vel0         : "+str(vel0)+" m/s")
  prevVel1 = vel1
  vel1 = float(ventset_obj.root_body.composite_body.state.trans.velocity[1])
  print("vel1         : "+str(vel1)+" m/s")
  prevVel2 = vel2
  vel2 = float(ventset_obj.root_body.composite_body.state.trans.velocity[2])
  print("vel2         : "+str(vel2)+" m/s")

def vel0Changed():
  global vel0, prevVel0
  return (vel0 != prevVel0)

def vel1Changed():
  global vel1, prevVel1
  return (vel1 != prevVel1)

def vel2Changed():
  global vel2, prevVel2
  return (vel2 != prevVel2)


def computeExpectedMasses():
  global initializedTank0Mass, initializedTank1Mass, vent0MassChange, vent1MassChange, vent2MassChange, vent3MassChange

  initializedTank0Mass = float(ventset_obj.tank[0].dynamic_properties.consumable_mass)
  initializedTank1Mass = float(ventset_obj.tank[1].dynamic_properties.consumable_mass)
  print('')
  print("tank0Mass = "+str(initializedTank0Mass)+" kg")
  print("tank1Mass = "+str(initializedTank1Mass)+" kg")
  
  # vent[0] is a dynamic vent; 'vent' class therefore changes mass
  vent0MassChange = float(ventset_obj.set_2_tanks.get_vent(0).get_duration()) * float(ventset_obj.set_2_tanks.get_vent(0).get_flowrate())
  # vent[1] is an impulse vent; 'simple_vent' class which does not change mass
  vent1MassChange = float(ventset_obj.set_2_tanks.get_vent(1).get_impulse_magnitude()) / float(ventset_obj.set_2_tanks.get_vent(1).get_exhaust_speed())
  # vent[2] is an impulse vent; 'vent' class therefore changes mass
  vent2MassChange = float(ventset_obj.extra_vent.get_duration()) * float(ventset_obj.extra_vent.get_flowrate())
  # vent[3] is a dynamic vent; 'simple_vent' class which does not change mass
  # no need to compute a new mass...

  print('')
  print("expected vent0MassChange = "+str(vent0MassChange)+" kg")
  print("expected vent1MassChange = "+str(vent1MassChange)+" kg")
  print("expected vent2MassChange = "+str(vent2MassChange)+" kg")
  print("expected vent3MassChange = "+str(vent3MassChange)+" kg")
  print('')


def impulseIndefiniteDuration(startTime, endTime, ventNum):
  global vent0MassChange, vent2MassChange
  if (ventNum == 0):
    vent0MassChange = (endTime - startTime) * float(ventset_obj.set_2_tanks.get_vent(0).get_flowrate())
    print("indefinite duration is on. new vent0MassChange = %lf kg"%vent0MassChange)
  if (ventNum == 2):
    vent2MassChange = (endTime - startTime) * float(ventset_obj.extra_vent.get_flowrate())
    print("indefinite duration is on. new vent2MassChange = %lf kg"%vent2MassChange)
