exec(open("Log_data/log_data.py").read())

print('\n***********************************************')
print("No error generated, but data is corrupt")
print("***********************************************")
trick.add_read(10,"""
testing_model.reload_data_indep_wrongsize()
""")

trick.stop(12)
