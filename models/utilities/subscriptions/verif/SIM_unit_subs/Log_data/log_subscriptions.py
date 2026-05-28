dr_group = trick.DRAscii("subs_verif")
dr_group.set_cycle(1.0)
dr_group.freq = trick.DR_Always

dr_group.add_variable("subs_verif.subs.enabled" )
dr_group.add_variable("subs_verif.subs.active" )
dr_group.add_variable("subs_verif.subs.num_subscriptions" )
dr_group.add_variable("subs_verif.subs.sub_pending")

trick.add_data_record_group(dr_group, trick.DR_Buffer)
