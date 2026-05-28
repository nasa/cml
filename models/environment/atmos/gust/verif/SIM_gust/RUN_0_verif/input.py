exec(open( "Log_data/log_data.py").read())

gust_unit.gust.gust_frame = trick.GustModel.TD_FRAME
gust_unit.gust.direction = [1,0,0]
gust_unit.gust.maximum_magnitude = 100.0
gust_unit.gust.period   = 10.0

trick.add_read(5.5, "gust_unit.gust.activate()")
trick.add_read(18.0, "gust_unit.gust.start = True")

trick.stop(30)
