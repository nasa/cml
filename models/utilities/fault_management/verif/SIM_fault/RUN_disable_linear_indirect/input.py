from Log_data.logging import log_default
log_default(0.1)

# Trigger set-up
test_object.trigger.Operator = trick.TriggerBase.GE
test_object.trigger.value = 1.0

# Fault set-up
test_object.fault_function.enabled = True
test_object.fault_function.type = trick.FaultFunctionBase.Linear
test_object.fault_function.nominal = 2.0
test_object.fault_function.rate = 0.4

trick.add_read(2,"test_object.fault_function.disable()")
trick.add_read(3.5,"test_object.fault_function.enabled = True")
trick.stop(5.0)
