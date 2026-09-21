exec(open("Modified_data/configure_vehicle.py").read())

def setDynamics(
    mngr,
    initPhase,
    driftDeadband,
    maxDeltaLatVelDP,
    driftRateMax,
    deadbandAccel,
    mnvrRateMax,
    mnvrAccelInput,
    rateTolerance,
    eulerTolerance
):
    for ii in range(3):
        mngr.perturbation.initPhase[ii] = initPhase[ii]
        mngr.perturbation.driftDeadbandInput[ii] = driftDeadband[ii]
        mngr.perturbation.maxDeltaLatVelDPInput[ii] = maxDeltaLatVelDP[ii]
        mngr.perturbation.driftRateMaxInput[ii] = driftRateMax[ii]
        mngr.perturbation.deadbandAccelInput[ii] = deadbandAccel[ii]
        mngr.maneuver.mnvrRateMaxInput = mnvrRateMax
        mngr.maneuver.mnvrAccelInput = mnvrAccelInput
        mngr.maneuver.rateTolerance = rateTolerance
        mngr.maneuver.eulerTolerance = eulerTolerance
    return

def setTargetAttitude():
    # 30 deg Yaw attitude (YPR)
    test.quatTargetRefToNominal.scalar =  0.96592583
    test.quatTargetRefToNominal.vector[0] = -0.25881905
    test.quatTargetRefToNominal.vector[1] = 0.0
    test.quatTargetRefToNominal.vector[2] = 0.0
    test.att_manager.maneuver.set_target_attitude(
        test.quatTargetRefToNominal
    )

setDynamics(
        mngr=test.att_manager,
        initPhase=[0.005, -0.005, 0.01],
        driftDeadband=[0.01, 0.01, 0.01],
        maxDeltaLatVelDP=[100.0, 0.003, 0.003],
        driftRateMax=[0.002, 0.002, 0.002],
        deadbandAccel=[0.01, 0.01, 0.01],
        mnvrRateMax=0.1,
        mnvrAccelInput=0.01,
        rateTolerance=0.05,
        eulerTolerance=0.05
    )
test.att_manager.perturbation.randSeed = 1
test.att_manager.maneuver.randSeed = 1
test.att_manager.perturbation.portName = "docking_port"
test.att_manager.perturbation.set_enforce_lat_vel( False)