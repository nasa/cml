exec(open("Log_data/log_data.py").read())

trick.add_read(5, "test.sample.buffer.set_buffer_size(5)")
trick.add_read(15, "test.sample.buffer.set_buffer_size(10)")
trick.add_read(22, "test.sample.buffer.set_buffer_size(3)")
trick.add_read(27, "test.sample.buffer.set_buffer_size(1)")
trick.add_read(29, "test.sample.buffer.set_buffer_size(5)")
trick.add_read(35, "test.sample.buffer.set_buffer_size(0)")
trick.add_read(35, "test.sample.buffer.set_buffer_size(11)")

test.sample.lookup_tag = 8.0
trick.add_read(31, "test.sample.lookup_tag =37.75 ")
trick.add_read(33, "test.sample.buffer.require_exact_tag = True")
trick.stop(35)
