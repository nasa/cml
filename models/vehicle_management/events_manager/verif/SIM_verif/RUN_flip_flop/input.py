drg1 = trick.DRAscii("events_manager_verif_vars")
drg1.set_cycle(1.0)
drg1.freq = trick.DR_Always
trick.add_data_record_group(drg1, trick.DR_Buffer)
events_manager_verif.logging_groups.add_to_list(drg1)
drg1.add_variable("events_manager_verif.base_bool")
drg1.add_variable("events_manager_verif.xx1")

events_manager_verif.framework.data_file_name = "../data/events_manager_ut_data.txt"
events_manager_verif.framework.vars_file_name = "../data/events_manager_ut_variables.txt"

trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)


trick.add_read(1.0,"events_manager_verif.base_bool = True")
trick.add_read(2.0,"events_manager_verif.base_bool = False")
trick.add_read(2.5,"events_manager_verif.base_bool = True")
trick.add_read(3.0,"events_manager_verif.manager.flip_flop.subscribe()")
trick.add_read(4.0,"events_manager_verif.base_bool = False")
trick.add_read(6.0,"events_manager_verif.base_bool = True")
trick.add_read(8.0,"events_manager_verif.base_bool = False")

trick.stop(8)
