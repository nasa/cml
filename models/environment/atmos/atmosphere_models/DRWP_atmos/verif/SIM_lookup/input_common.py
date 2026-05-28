exec(open( "Log_data/log_lu_winds.py").read())
# Set message publication level to see inform() messages
trick.CMLMessage.set_publish_level( trick.CMLMessage.Inform)

luwinds.lookup_table_winds.subscribe()
trick.stop(95)
