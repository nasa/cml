'''
Run according to the config specified in verif_eevnts_manager.hh
with default options.

Note -- at t=5, the event arms but cannot trigger because default
behavior prevents triggering on the same cycle as arming.
See RUN_1b for change to this behavior.
'''
exec(open("Log_data/log_data.py").read())
verif.framework.vars_file_name = "Unit_test_data/vars.txt"
verif.framework.data_file_name = "Unit_test_data/data.txt"

verif.manager.enabled = True
trick.CMLMessage.set_publish_level( trick.CMLMessage.Debug)

verif.manager.event.subscribe();

trick.stop(10)
