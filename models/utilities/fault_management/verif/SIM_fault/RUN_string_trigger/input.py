from Log_data.logging import log_default
log_default(0.1)

# Trigger set-up
test_object.trigger_str.Operator = trick.TriggerBase.EQ
test_object.trigger_str.value = "go"
test_object.control_string = "stop"

# Fault set-up
test_object.fault_function.add_trigger_group(test_object.trigger_str_group);
test_object.fault_function.enabled = True
test_object.fault_function.type = trick.FaultFunctionBase.Linear
test_object.fault_function.nominal = 2.0
test_object.fault_function.rate = 0.4

trick.add_read(2,"test_object.control_string = 'go'")
trick.add_read(3,"test_object.trigger_str.set_value('stop')")
trick.add_read(4,"test_object.control_string = 'stop'")
trick.stop(5.0)
