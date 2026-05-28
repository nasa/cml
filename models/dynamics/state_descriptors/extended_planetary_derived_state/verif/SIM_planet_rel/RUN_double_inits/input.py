# Purpose:
# - verify simple returns with no action on inactive update calls.
# - verify initialization of launch-range when range-safety is subscribed
#     post-init (see ExtendedPlanetaryDerivedState::subscribe_range_safety())
# - verify protection against re-initialization on plumbline and
#     boost-reference sub-models
# These tests are not verifiable by data products because the main purpose is
# to confirm no-ops.  They must be checked manually through debugging

exec(open("Modified_data/env_setup.py").read())
exec(open("Log_data/log_base.py").read())
log_control_flags()


vehicle.planet_rel_state.subscribe_boost_reference()
vehicle.planet_rel_state.subscribe_plumbline()

trick.add_read(0,"""
# test update of inactive model, should return:
vehicle.planet_rel_state.update()

# test launch-range sub-model initialization resulting from range-safety
# subscription following model initialization
vehicle.planet_rel_state.subscribe_range_safety()

# test re-initialization, should push out to re-initialize sub-models and those
# that have protection against re-initialization (i.e. plumbline and
# boost-reference) should take no action:
vehicle.planet_rel_state.initialize( vehicle.body, dynamics.dyn_manager)
""")

trick.stop(0)
