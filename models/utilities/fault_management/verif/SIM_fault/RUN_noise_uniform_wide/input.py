from Log_data.logging import log_default
log_default(0.1)

# Trigger set-up
test_object.trigger.Operator = trick.TriggerBase.GE
test_object.trigger.value = 1.0

# Fault set-up
test_object.fault_noise.enabled = True
test_object.fault_noise.noise.initialize_flat(0, 10, 1)

trick.stop(5.0)
