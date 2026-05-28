exec(open("RUN_1a_simple_sequence/input.py").read())
print("\n"
"*****************************************************************\n"
" Message status is send: %g (0)\n"
"*****************************************************************"
%test.indirect_set.status_is_send())
test.indirect_set.get_message_status()
trick.stop(0)
