exec(open("RUN_frame_equatorial/input.py").read())
# Trigger the error() in CorrelatedStateDispersion::transform_ra_dec_fp
verif.true_position = [0.0, 0.0, 0.0]

print ( \
'***********************************************************************\n' \
' Error follows when the attempt to generate RA, DEC and attitude fails.\n' \
' Result is to set RA = DEC=0 and align with NED\n' \
'***********************************************************************\n')
