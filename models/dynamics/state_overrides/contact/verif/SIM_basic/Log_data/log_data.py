dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(0.5) # log unit-test value on integer, then override-value on half-integer times
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("contact_obj.contact.active")
dr_group.add_variable("contact_obj.contact.contact_detected")

for ii in range(3) :
  dr_group.add_variable("contact_obj.override.composite_body.state.trans.position[" + str(ii) + "]")

for ii in range(3) :
  dr_group.add_variable("contact_obj.override.composite_body.state.trans.velocity[" + str(ii) + "]")

dr_group.add_variable("contact_obj.override.composite_body.state.rot.Q_parent_this.scalar")
for ii in range(3) :
  dr_group.add_variable("contact_obj.override.composite_body.state.rot.Q_parent_this.vector[" + str(ii) + "]")

for ii in range(3) :
  dr_group.add_variable("contact_obj.override.composite_body.state.rot.ang_vel_this[" + str(ii) + "]")

