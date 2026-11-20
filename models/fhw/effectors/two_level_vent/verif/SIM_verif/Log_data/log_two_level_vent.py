def set_group(name):
    dr_group = trick.DRAscii(name)
    dr_group.set_cycle(1.0)
    dr_group.set_freq(trick.DR_Always)
    trick.add_data_record_group(dr_group, trick.DR_Buffer)

    return dr_group

def log_vec(dr_group, size, var):
    for ii in range(size):
        dr_group.add_variable(var + f"[{ii}]")

def log_vent_verbose(vent):
    dr_group = set_group(vent)
    for param in ['high_venting','force_mag','high_duration','high_wait_time','sched_onTime','dv_mag', 'start_time_high']:
        dr_group.add_variable(f'two_level_vent_model_obj.{vent}.{param}')

def log_vent_dur(vent):
    dr_group = set_group(vent)
    for param in ['high_venting','high_duration','dv_mag','sched_onTime']:
        dr_group.add_variable(f'two_level_vent_model_obj.{vent}.{param}')

def log_vent_waitTime(vent):
    dr_group = set_group(vent)
    for param in ['high_venting','high_wait_time', 'sched_onTime']:
        dr_group.add_variable(f'two_level_vent_model_obj.{vent}.{param}')

def log_vent_force(vent):
    log_vec(dr_group, 3, f"two_level_vent_model_obj.{vent}.force")

def log_vent_param(vent, parameters):
    dr_group = set_group(vent)
    for param in parameters:
        dr_group.add_variable(f'two_level_vent_model_obj.{vent}.{param}')

def log_vent_set(set):
    dr_group = set_group(set)
    log_vec(dr_group, 3, f"two_level_vent_model_obj.{set}.force")
    log_vec(dr_group, 3, f"two_level_vent_model_obj.{set}.torque")

def log_all():
    for vent in ['vent1', 'vent2']:
        log_vent_verbose(vent)    
    log_vent_set("two_level_vent_set1")
    log_vent_set("two_level_vent_set2")
