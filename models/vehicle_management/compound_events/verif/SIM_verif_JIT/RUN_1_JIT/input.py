'''
Because the model relies heavily on class-templates, it is recommended that
input-file configurations be implemented in a Just-In_Time (JIT) C-code
input file rather than a SWIG-interpreted Python-code input file. However, at
this time, we do not have a solution for extracting code-coverage metrics
when using JIT input files.

Consequently, we have implemented the test run as a JIT-input files in
input.cpp ***AS A DEVELOPMENT TEMPLATE ONLY *** to provide
a guide as to how to implement the features of this model in an integrated
simulation. However, the automated test runs ***DOES NOT USE*** this JIT-compiled
implementations because we need to get code-coverage metrics from the
downstream Jenkins jobs.

The contents of the JIT-input files are copied into a method
   jit_input_replacement()
in the S_define, which can be pre-compiled and called from this Python input
file.
This step of copying JIT-input file content into the S_define for compilation with
the sim is wholly unnecessary in an integrated simulation environment.
**IT IS INCLUDED HERE FOR THE SOLE PURPOSE OF COLLECTING CODE COVERAGE
METRICS**. In an integrated environment, the JIT-input file could be used, and
integrated development should be based on that example.
'''
exec(open("Log_data/log_data.py").read())
verif.framework.vars_file_name = "Unit_test_data/vars.txt"
verif.framework.data_file_name = "Unit_test_data/data.txt"


'Here is the call that replaces input.cpp'
verif.jit_input_replacement()

verif.manager.enabled = True
trick.CMLMessage.set_publish_level( trick.CMLMessage.Debug)

trick.stop(10)
