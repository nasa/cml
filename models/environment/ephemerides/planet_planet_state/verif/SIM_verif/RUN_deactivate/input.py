exec(open("RUN_subsc_off/input.py").read())

trick.add_read(200.0, """
planet_rel_state.relstate_name.subscribe()
planet_rel_state.relstate_direct.subscribe()
""")
trick.add_read(800.0, """
planet_rel_state.relstate_name.unsubscribe()
planet_rel_state.relstate_direct.unsubscribe()
""")
