# Check the application of an impulse when the vent-body is root,
exec(open("RUN_impulse_nominal/input.py").read())
# reverse the order of attaching, should produce the same composite body.
ventset_obj.attach.set_parent_body(ventset_obj.vent_body)
ventset_obj.attach.set_subject_body(ventset_obj.root_body)
