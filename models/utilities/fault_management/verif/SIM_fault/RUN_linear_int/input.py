from Log_data.logging import log_default
log_default(0.1)

# Trigger set-up
test_object.trigger.Operator = trick.TriggerBase.GE
test_object.trigger.value = 1.0

# Fault set-up
test_object.fault_function.name = 'Linear'
test_object.fault_function_int.enabled = True
test_object.fault_function_int.type = trick.FaultFunctionBase.Linear
test_object.fault_function_int.nominal = 2.0
test_object.fault_function_int.rate = -0.4

trick.stop(5.0)
