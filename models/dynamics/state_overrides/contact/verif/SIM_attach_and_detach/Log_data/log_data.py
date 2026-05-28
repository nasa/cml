dr_group = trick.DRAscii("test_data")
dr_group.set_cycle(0.5)
dr_group.freq = trick.DR_Always
trick.add_data_record_group(dr_group, trick.DR_Buffer)

dr_group.add_variable("contact_obj.contact.active")
dr_group.add_variable("contact_obj.contact.contact_detected")

for ii in range(3):
  dr_group.add_variable("contact_obj.override.core_body.state.trans.position[%d]" %ii)

dr_group.add_variable("contact_obj.override.core_body.state.rot.Q_parent_this.scalar")
for ii in range(3):
  dr_group.add_variable("contact_obj.override.core_body.state.rot.Q_parent_this.vector[%d]" %ii)

for ii in range(3):
  dr_group.add_variable("contact_obj.root.core_body.state.trans.position[%d]" %ii)
