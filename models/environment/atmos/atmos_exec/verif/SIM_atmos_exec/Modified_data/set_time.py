def set_time_utc(year, month, day, hour, minute, second):
  exec(open("Modified_data/time_default_config.py").read())
  
  jeod_time.time_utc.calendar_year = year
  jeod_time.time_utc.calendar_month = month
  jeod_time.time_utc.calendar_day = day
  jeod_time.time_utc.calendar_hour = hour
  jeod_time.time_utc.calendar_minute = minute
  jeod_time.time_utc.calendar_second = second
  return;
