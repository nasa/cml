veh_action.detach.springs.enabled      = True
veh_action.detach.springs.num_springs  = 1
veh_action.detach.springs.spring_array = trick.sim_services.alloc_type(1,
                                                   "SimpleSpringIndividual")


#Spring 1
veh_action.detach.springs.spring_array[0].spring_constant    = 20
veh_action.detach.springs.spring_array[0].damping_constant   = 0.0
veh_action.detach.springs.spring_array[0].stroke             = 0.05
veh_action.detach.springs.spring_array[0].final_compression  = 0.0
veh_action.detach.springs.spring_array[0].position_action    = [0.0, 0.0, 0.0]
veh_action.detach.springs.spring_array[0].position_reaction  = [0.0, 0.0, 0.0]
