Concept:
Make a 2-trigger event, 1 watching a int and 1 a bool, require-all
vary the int up and down

make the event multi-shot, log event-triggered.

test each of the comparison_logic operations, use the bool to control whether
satisfying the int trigger modifies the reference when the trigger does not
trigger.




The model provides some on-the-fly creation methods to create and arrange new
events and triggers. The trigger creation is template driven and not available
to SWIG.

This sim uses SWIG input files, so the event manager must be built
independently as part of the sim compilation.

A parallel verif sim evaluates the on-the-fly creation and configuration
mechanisms and is limited to JIT-input file users.

The two sims were separated to ensure that the JIT-testing was not
inappropriately assisted by a pre-compiled form of the same content.
