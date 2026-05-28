# Aero executive is disabled, when model goes to initialize and activate, it
# will return an error about being disabled.
# Executive will return immediately.
exec(open("input_common.py").read())
aero.interface.executive.change_table(aero.table_SYM_LDm)
print("\n"
"***** disabling executive now             **********************************\n"
"***** 3 errors follow:                    **********************************\n"
"****** - subscription to a disabled model **********************************\n"
"****** - interface fails due to executive failing to subscribe *************\n"
"****** - interface attempts to unsubscribe executive ***********************\n")
aero.interface.executive.disable()
trick.stop(0)
