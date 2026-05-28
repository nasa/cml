#This unit test is to increase code coverage for the model.
#Specifically to hit line 90. 

exec(open("Log_data/log_data.py").read())
log_test_data(1.0)

vehicle.detach_B_from_A.apply(dynamics.dyn_manager)
trick.sim_services.exec_set_terminate_time(10)
