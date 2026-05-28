exec(open("Log_data/log_data.py").read())

test.table_interp.subscribe()
test.table_prev.subscribe()
test.table_next.subscribe()
test.table_floor.subscribe()
test.table_ceil.subscribe()
test.table_round.subscribe()
test.table_wrap.subscribe()
test.table_reverse.subscribe()
test.table_single_array.subscribe()
test.table_single_vec.subscribe()

trick.stop(50)
