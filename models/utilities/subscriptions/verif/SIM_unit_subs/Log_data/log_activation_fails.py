dr_group = trick.DRAscii("subs_verif")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("subs_verif.activation_fails.initialize_on_failed_activation")
dr_group.add_variable("subs_verif.activation_fails.initialized" )
dr_group.add_variable("subs_verif.activation_fails.active" )
dr_group.add_variable("subs_verif.activation_fails.sub_pending")
dr_group.add_variable("subs_verif.activation_fails.num_subscriptions")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
