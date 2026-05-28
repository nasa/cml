# The output of this configuration should be:
#    out = 11 - |i-1| - 5|j|
#   where i = independent % 2               (mod(independent,2))
#         j = ((independent +2) % 4) - 2
exec(open("Log_data/log_data.py").read())

test.table_set.subscribe()

trick.stop(12)
