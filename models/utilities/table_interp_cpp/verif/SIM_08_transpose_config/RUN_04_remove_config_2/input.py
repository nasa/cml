# remove config successfully
print("*** Remove config succesfully ******************************************")
test.manager.remove_config( test.config1)

print("*** fail to remove a second time, no match ****")
test.manager.remove_config( test.config1)

print("*** add config twice ***")
print("*** cannot remove; ambiguous match ***")
test.manager.add_config( test.config1)
test.manager.add_config( test.config1)
test.manager.remove_config( test.config1)

trick.stop(0)
