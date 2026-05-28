The model provides some on-the-fly creation methods to create and arrange new
events and triggers. The trigger creation is template driven and not available
to SWIG. So this sim is dedicated to JIT-input file users.

Note that Trick-logging of the internal mechanisms of the Event itself
is challenging in this environment because we do not have direct access to the
CompoundEvent or any of its components.

A parallel verif sim is used for SWIG-input file users, and requires that the
triggers be pre-built.
