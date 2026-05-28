exec(open("Log_data/log_data.py").read())
verif.framework.vars_file_name = "Unit_test_data/vars.txt"
verif.framework.data_file_name = "Unit_test_data/data_1.txt"
trick.CMLMessage.set_publish_level( trick.CMLMessage.Debug)
verif.manager.enabled = True
verif.manager.event.subscribe()

'''
set initial value for sim_int; this is important for the MAX and MIN
comparison-logic because the initial MAX/MIN are taken from the initial values
of the monitored variable.
'''
verif.sim_data.sim_int = 1
trick.stop(16)
