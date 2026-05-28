# remove config successfully
print("*** Remove config succesfully ******************************************")
print("*** process_data will fail to create a table without a configuration ***")
print("*** initialization will fail without a table. **************************")
test.manager.remove_config( test.config1)
test.process_data()
test.initialize()
trick.stop(0)
