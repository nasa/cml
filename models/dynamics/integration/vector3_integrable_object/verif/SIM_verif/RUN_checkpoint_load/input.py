exec(open("RUN_internal_no_update/input.py").read())

# at time 5.0, load the checkpoint
trick.add_read(5.0, """trick.load_checkpoint("CHECKPOINT/chkpnt_15.000000")""")
