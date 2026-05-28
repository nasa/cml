This folder represents how a specific project might extend the CML model to
provide an exteneded manager with built-in (compilable) definitions for groups.

Because the model relies heavily on class-templates, it is recommended that
input-file configurations be implemented in a Just-In_Time (JIT) C-code
input file rather than a SWIG-interpreted Python-code input file. However, at
this time, we do not have a solution for extracting code-coverage metrics
when using JIT input files.

Consequently, we have implemented the test runs as JIT-input files in
RUN*/input_jit.cc ***AS A DEVELOPMENT TEMPLATE ONLY *** to provide
a guide as to how to implement the features of this model in an integrated
simulation. However, the automated test runs ***DO NOT USE*** these JIT-compiled
implementations because we need to get code-coverage metrics from the
downstream Jenkins jobs.

The contents of the JIT-input files are consolidated into a class in
Project_implementation and compiled into the sim.  Each run also has a
Python input file that is fed to the automated system; these runs execute
the pre-compiled forms of the JIT-input files.
This step of copying JIT-input file content into a class for compilation with
the sim is wholly unnecessary in an integrated simulation environment,
**IT IS INCLUDED HERE FOR THE SOLE PURPOSE OF COLLECTING CODE COVERAGE METRICS**.
