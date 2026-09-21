'''
This test checks that a warning is issued when 
    - perturbation submodel is subscribed twice
    - maneuver submodel is subscribed twice
    - setting a NULL pointer as the Ref reference frame
    - setting a Ref reference frame after model activation
'''

exec(open("Modified_data/nominalInput.py").read())
test.att_manager.subscribe_perturbation()

print("""
################################################################
Setting Reference frame state to a NULL pointer.
Print warning and pass.
################################################################
""")
test.att_manager.set_ref_frame_state(test.null_frame_state)


trick.add_read(1.0, '''print("""
################################################################
Subscribing perturbation twice.
Print warning and ignore.
################################################################
""")''')
# Subscribe perturbation twice
trick.add_read(1.0, 
"test.att_manager.subscribe_perturbation()"
)

trick.add_read(1.0, '''print("""
################################################################
Subscribing maneuver twice.
Print warning and ignore.
################################################################
""")''')
# Subscribe maneuver twice
trick.add_read(1.0, 
"test.att_manager.subscribe_maneuver()\n"
"test.att_manager.subscribe_maneuver()"
)

trick.add_read(1.0, '''print("""
################################################################
Setting Reference frame state after model has already been activated.
Print warning and pass.
################################################################
""")''')
trick.add_read(1.0, 
"test.att_manager.set_ref_frame_state(test.null_frame_state)"
)

trick.stop(1.0)