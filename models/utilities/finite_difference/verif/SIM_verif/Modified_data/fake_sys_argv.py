# Since upgrading to python3, the unittest module depends on sys.argv
# existing, but trick/swig appears to remove it internally. Fake out
# the existence of sys.argv so unittest can complete.
# -Jordan 6/22
sys.argv = []
sys.argv.append('fakearg')

