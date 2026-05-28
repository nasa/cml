from Log_data.logging import log_default
log_default(0.1)

# Trigger set-up
test_object.trigger.Operator = trick.TriggerBase.GE
test_object.trigger.value = 1.0

# Fault set-up
test_object.fault_stale.enabled = True
# Make var into a linear function
test_object.advance_var = True

trick.add_read(2,"test_object.fault_stale.disable()")
trick.add_read(3.5,"test_object.fault_stale.enabled = True")
trick.stop(5.0)
