#!/usr/bin/env python3
import os, sys, inspect, argparse, textwrap, time, pathlib, math, glob
thisFileDir = os.path.dirname(os.path.abspath(inspect.getsourcefile(lambda:0)))
def _parse_args():
    descr = '''
    Build, run, and compare all CML unit sims as specified in the TrickOps
    YAML file given by --config.  This script requires visibility to a pre-built
    Trick directory (25.0.2 or later) and a JEOD directory (5.0 or later).
    Returns zero if all tests pass, nonzero otherwise.
    '''
    parser = argparse.ArgumentParser(description=descr,
      formatter_class=argparse.ArgumentDefaultsHelpFormatter )

    action_choices = ['libcml', 'builds', 'runs', 'comparisons', 'all']
    parser.add_argument( 'action', nargs='*', choices=action_choices,
      default='all', help='Stages of test process to run.')

    parser.add_argument( "--trick", default=os.getenv('TRICK_HOME'),
      help="Path to top level of trick.")
    parser.add_argument( "--jeod", default=os.getenv('JEOD_HOME'),
      help="Path to top level of jeod.")

    parser.add_argument( "-c", "--config",
      default=os.path.join(thisFileDir,'default.yml'),
      help="YAML file containing the jobs to run.")
    parser.add_argument( "-q", "--quiet", action="store_true",
      help="Suppress progress bars. Recommended for CI usage.")
    parser.add_argument( "-j", "--cpus", default=6, type=int,
      choices=range(1,100), metavar="[1-100]",
      help="Total number of cpus to use when building and running sims.")
    parser.add_argument( "-m", "--model", dest="model",
      metavar="model_dir", nargs='+', default=[],
      help="Specify directories of the models to test by model_dir key. "
      "If not specified, all the models found in the config file are used.")

    args = parser.parse_args()

    if args.action == 'all' or 'all' in args.action:
      args.action = [a for a in action_choices if a != 'all']

    return args

def _verify_args(args):
    '''
    Ensure Trick and JEOD areas exist and libs are built. If they aren't, all tests will fail
    '''
    if not args.trick or not os.path.exists(args.trick):
        msg = ("Unable to locate trick directory. Define TRICK_HOME in your environment"
          " or use --trick=/path/to/trick/ to specify a path to a pre-built Trick"
          " directory (Requires version 19.5.1 or later)")
        raise RuntimeError(msg)
    if (not os.path.exists(os.path.join(args.trick, 'lib64')) or
        not os.listdir(os.path.join(args.trick,'lib64'))):
        msg = (f"Specified trick directory {args.trick} is missing built libraries. "
          "Build trick for this platform to resolve this error.")
        raise RuntimeError(msg)
    if not args.jeod or not os.path.exists(args.jeod):
        msg = ("Unable to locate JEOD directory. Define JEOD_HOME in your environment"
          " or use --jeod=/path/to/jeod/ to specify a path to a JEOD"
          " directory (Requires version 3.4 or later)")
        raise RuntimeError(msg)
    # We need to know what TRICK_HOST_CPU is to find the libraries, so get that first
    cmd = ( os.path.join(args.trick, 'bin/trick-gte') + " TRICK_HOST_CPU")
    sys.path.append(os.path.abspath(os.path.join(args.trick, 'share/trick/trickops/')))
    from WorkflowCommon import run_subprocess
    trick_host_cpu = run_subprocess(command=cmd,m_shell=True).stdout.strip().split("\n")[-1]
    if not os.path.isfile(os.path.join(args.trick, 'lib64/libtrick.a')):
        raise RuntimeError(f"Trick hasn't been built. Build {args.trick} and try again.")
    if not os.path.isfile(os.path.join(args.jeod, 'lib_jeod_' + trick_host_cpu + '/libjeod.a' )):
        raise RuntimeError(f"JEOD library under {args.jeod} hasn't been built. See README.md for instructions and try again.")
    if not os.path.isfile(os.path.join(args.jeod, f'trickified/trickified_jeod_{trick_host_cpu}.o')):
        raise RuntimeError(f"JEOD trickified library under {args.jeod} hasn't been built. See README.md for instructions and try again.")
    ephem_files = glob.glob(os.path.join(args.jeod, f'lib_jeod_{trick_host_cpu}', 'de4xx_lib', 'libde*'))
    if len(ephem_files) < 1:
        raise RuntimeError(f"JEOD ephemeris files under {args.jeod} haven't been built. See README.md for instructions and try again.")

    return args  

# Argparsing must happen at global scope since we can't import TrickOps without
# figuring out the location of Trick, which can be passed in to this script.
args = _verify_args(_parse_args())

# Import TrickWorkflow content provided by trick
try:
  from WorkflowCommon import *
  from TrickWorkflow import *
except Exception as e:
  msg = (f"Unable to import TrickOps modules from {args.trick}. Make sure you are"
  " using a valid Trick directory and have activated the python3 virtual environment."
  " See bin/README.md for details.")
  raise RuntimeError(msg) from e

class CmlTestWorkflow(TrickWorkflow):
    def __init__( self):
        self.simultaneous_builds = args.cpus
        args.model = [ os.path.normpath(m) for m in args.model ]
        TrickWorkflow.__init__(self, project_top_level=os.path.abspath(os.path.join(thisFileDir, '..')),
          log_dir=os.path.join(thisFileDir, 'logs'),
          trick_dir=args.trick, config_file=args.config,
          cpus=self.simultaneous_builds, quiet=args.quiet)
        self.sims_to_test = self._get_sims_to_test()

    def _get_sims_to_test(self):
        sims = []
        if len(args.model) > 0:
          all_sims = self.get_sims()
          for sim in all_sims:
            if 'model_dir' in self.config[sim.name] and os.path.normpath(self.config[sim.name]['model_dir']) in args.model:
              sims.append(sim)
        else:
           sims = self.get_sims()
        return sims

    def _get_libcml_job(self):
        '''
        Create a job to build libcml in developer mode.
        '''
        command = 'cmake --preset dev --fresh && cmake --build --preset dev'
        log_file = os.path.join(self.log_dir, 'build_libcml.txt')
        return [Job(name='Build libcml', command=command, log_file=log_file)]

    def run( self):
      if 'libcml' in args.action:
        libcml_status = self.execute_jobs(self._get_libcml_job(),
          max_concurrent=self.simultaneous_builds, header='Building libcml.')
        if self.quiet:
          print('Finished libcml build stage.')

      if 'builds' in args.action:
        build_jobs = [ s.get_build_job() for s in self.sims_to_test ]
        builds_status = self.execute_jobs(build_jobs,
          max_concurrent=self.simultaneous_builds, header='Executing all sim builds.')
        if self.quiet:
          print('Finished sim builds stage.')

      if 'runs' in args.action:
        run_jobs = [ s.get_run_jobs() for s in self.sims_to_test ]
        run_jobs = [item for sublist in run_jobs for item in sublist] # Flatten list of lists
        # If these jobs are of type SingleRun and self.quiet is True, tell the jobs to
        # skip the variable server connection logic. TODO: Implement this in sim.get_run_jobs in
        # TrickOps, we get use get_jobs() here - Jordan 7/2023
        for job in run_jobs:
            if self.quiet and isinstance(job, SingleRun):
                job.set_use_var_server(False)
        runs_status = self.execute_jobs(run_jobs,
          max_concurrent=self.simultaneous_builds, header='Executing all sim runs.')
        if self.quiet:
          print('Finished sim runs stage.')

      if 'comparisons' in args.action:
        compare_failure = any([s.compare() for s in self.sims_to_test])  # Run all comparisons
        if self.quiet:
          print('Finished comparisons stage.')

      self.report()           # Print Verbose report
      return (self.status_summary() == 'FAILURE')

if __name__ == "__main__":
    sys.exit(CmlTestWorkflow().run())
