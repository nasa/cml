dynamics.dyn_manager_init.sim_integ_opt = trick.sim_services.Runge_Kutta_4

exec(open( "Log_data/log_data.py").read())
log_test_data( 1 )

trick.sim_services.exec_set_terminate_time(100)
