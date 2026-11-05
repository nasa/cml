'''
This test checks that a warning is issued when:
    - smoothing flag is true and maneuver acceleration is less or equal to zero.
    Resets the flag to false.
    - setting smoothing when a maneuver is already active.
'''
exec(open("Modified_data/nominalInput.py").read())

test.att_manager.subscribe_maneuver()

print("""
################################################################
Smoothing with zero acceleration. Turns off smoothing option
################################################################
      """)
test.att_manager.maneuver.set_smoothing(True)
test.att_manager.maneuver.mnvrAccelInput = 0.0

trick.add_read(0.0,"""print('''
################################################################
Turning smoothing on when maneuver active. Ignore command
################################################################''')""")
trick.add_read(0.0, 'test.att_manager.maneuver.set_smoothing(True)')

trick.stop(0.0)