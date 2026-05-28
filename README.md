# Welcome to the Common Model Library

The Common Model Library (CML) is designed to be a common location to store and share Trick-based and JEOD-based models that have common applicability to multiple simulations in the NASA community. The origin of the CML project is currently located here: https://github.com/nasa/cml

[[_TOC_]]

# Dependencies

## System tools

CML models are largely C++ source code and accompanying python input files. Although not strictly required, we recommend using CML with the following tools:

1. `gcc` version 8.5.0 or higher
2. `python3` version 3.6.8 or higher
3. `swig` 3.0.12 or higher
4. `cmake` 3.25 or higher

## C++ Standard

CML requires that you build and link against it using the ISO/IEC 14882 C++17 standard or newer.
While some models may build with an older C++ standard, CML does not guarantee support for
standards older than C++17.

## Trick and JEOD

In order to use CML models, your workflow must have access to compatible versions of [Trick](https://github.com/nasa/trick/). Most (but not all) models also have dependencies on JEOD. Currently, the supported versions are:

* [Trick 25.0.2](https://github.com/nasa/trick)+ (must be built for the platform you intend to use CML on). We recommend using `export PYTHON_VERSION=3` and pointing Trick to SWIG version 3.X or higher, per the recommended dependencies in the Trick documentation.
* [JEOD 5.0](https://github.com/nasa/jeod) or newer (must be built into libraries for the same platform)
    - `lib_jeod_${TRICK_HOST_CPU}/libjeod.a`
    - `trickified/trickified_jeod_${TRICK_HOST_CPU}.o`
    - `lib_jeod_${TRICK_HOST_CPU}/de4xx_lib/libde*.so`

To build JEOD into the needed libraries, follow the instructions in the JEOD documentation. Here's an example of what that looks like:

```bash
# MAKE SURE TO DEFINE TRICK_HOME AND JEOD_HOME APPROPRIATELY!
export PATH=${TRICK_HOME}/bin:${PATH}
cd ${JEOD_HOME}
make -f bin/jeod/makefile TRICK_BUILD=1
# If successful, you should now see libjeod.a under jeod/lib_jeod_${TRICK_HOST_CPU}/ and
# various libde*.so libraries under jeod/lib_jeod_${TRICK_HOST_CPU}/de4xx_lib/
# Now we can build the trickified JEOD library
cd ${JEOD_HOME}/trickified && make
# If successful, you should now see trickified_jeod_${TRICK_HOST_CPU}.o under jeod/trickified/
```
CML also uses [TrickOps](https://nasa.github.io/trick/documentation/miscellaneous_trick_tools/TrickOps.html) for testing which is a `python3` module that travels with Trick under the `trick/share/trick/trickops/` directory.

# Environment

The CML `.bashrc` is intended to provide the environment needed for CML core development and testing. From a `bash` shell, simply `. .bashrc` to activate it. This file will print warnings if `JEOD_HOME` and `TRICK_HOME` are not found, or aren't suitably configured in the user environment. It will also look for the `python3` virtual environment which is needed for internal testing of CML models.

Note that the `.bashrc` inclusded in this project provides the environment for **this project** - we do not recommend that CML using projects source this file directly. Instead, use the `.bashrc` as an example of what CML expects to be defined and ensure your project's environment provides those definitions accordingly.

For CML core testing and development, we suggest creating an alias in your `~/.bashrc` to manage the environment for CML. For example:

```bash
alias cml='export TRICK_HOME=/<path_to>/trick; export PATH="${PATH}:$TRICK_HOME/bin"; export JEOD_HOME=/<path_to>/jeod; cd /<path_to>/cml/; . ./.bashrc'
```
With this alias active and `<path_to>` filled out appropriately, you can just run `cml` and your shell will be configured for basic CML usage.

# Using the CML in your project

Currently, the recommended path for using CML source code in an external project is to:

1. Clone the `cml/` project from this project's "true origin" URL in your local workspace
2. Push the local workspace `cml/` to a new remote under your project's namespace/control. This acts as your project-specific fork of CML
3. Add the fork from (2) as a git submodule or git subtree to your project
4. Your project adjusts its environment to define `CML_HOME` appropriately.
5. Developers commit project-specific C++ derived implementations in their repository which inherit from generic implementations in `cml/`. **DO NOT COMMIT CONTENT TO CML UNLESS IT IS GENERIC, FREE OF CUI AND ITAR CONTENT, AND INTENDED TO BE FED BACK TO THE CML "TRUE ORIGIN"**
6. If `cml/` bugs are found, or new generic capability needs to be added, developers interact with their project-specific `cml/` fork for making changes
7. If (6) happens, project integrators occasionally feed content back to the "true origin" via Merge Requests and CML workflow process. More information in the Contributing section below.

## Building CML

CML uses CMake as its preferred meta-build tool. To build CML as a user, run

```bash
cmake --workflow --preset user-release
```

Other workflow presets exist. For more information, run

```bash
cmake --workflow --list-presets
```

To include CML in your sim, include the following line in your `S_overrides.mk`:

```makefile
include ${CML_HOME}/mkspecs/cml.mk
```

See [cml.mk](mkspecs/cml.mk) for a list of build options that influence how CML is built for your sim. Users have the option of building CML separately and linking against the prebuilt libraries when building their main simulation, or may build CML as part of their main sim build process. Note that when building multiple CML-using sims in parallel, we recommend that you build CML separately using one of the `cmake` commands above prior to building your simulations. Multiple simultaneous builds may result in the `libcml` binary being overridden while another simulation is attempting to link against it.

# Design Overview

The models provided by cml live under the `models/` subdirectory, and those subdirectories should contain verification tests which test the model in a standalone manner. Ideally, a model is tested both as an individual code unit and as part of a Trick simulation.

There are common make rules under the `mkspecs` subdirectory which allow the aforementioned verif simulations to `include` a common rule set in their respective `S_overrides.mk` files. The `bin/` directory contains CI and testing infrastructure.

## Regarding `#include "jeod/models/..."` in CML headers and `-I${JEOD_HOME}/..` and `-I${CML_HOME}/..`in `cml.mk`

The choice to prepend the full path of jeod and cml header files inside CML model header files that use them is deliberate as a safety mechanism. Historically the paths under `jeod/models/` have acted as a directory structure template for other projects that also use `jeod`. This similar directory structure combined with identical class naming and/or header guards has historically lead to to headaches associated with `#include`ing the wrong header file when a sim uses both JEOD and CML. In order to prevent this issue, we deliberately prepend the `jeod/models/` string in our CML model `#include` lines to guarantee correctness when compiling.  Similarly, the full path including `cml/models/<rest_of_path>`  is intended to be used by CML-using groups.

Accordingly, this means that `-I${JEOD_HOME}/..` and `-I${CML_HOME}/..` must be provided in `TRICK_CXXFLAGS`, technically allowing CML-using sims to search the path above `jeod/` and above `cml/` for header files when compiling.  Most (but not all) projects track CML and JEOD as a subrepo of their own project, making "one layer up" visiblity not an issue.

The CML `.bashrc` will automatically detect if `JEOD_HOME` does not have the required `jeod` basename and recommend a solution using symbolic links in this manner:

# Maturity Level
Within each model's README.md file, the model's maturity level is specified:

0. Either the model is obsolete or incomplete. Typically is well developed conceptually but may not build or run as intended. Model should be used as reference only.
1. Model builds and runs. Typically lacks documentation and verification. Model may be used, but at your own risk.
2. Model has undergone some preliminary review and/or verification but unit testing is minimal to non-existent. Documentation may be started, but generally lacking in detail and reliability. Model may be used, with caution.
3. Model has substantial documentation and reliable verification test cases that can be implemented as part of the project's unit-test framework. Model has not been independently verified.
4. Model development is complete, including comprehensive verification and documentation. Independent verification has not been completed. Level-4 may indicate a previously-level-5 model has since been edited and is awaiting a delta-verification review.
5. Model is mature, it has been comprehensively documented and independently verified.

# Contributing

See the [CONTRIBUTING](CONTRIBUTING.md) guide for more details.

## The `bin/` directory

This directory contains testing infrastructure for the CML project. None of the content here is needed for CML-model-using groups, it is only for those contributing to the CML repository and testing the models therein.

## Getting a python3 virtual environment (`.venv`)

The scripts which manage testing CML content leverage TrickOps modules which themselves require a valid `python3` virtual environment to be created and activated with content defined by `${TRICK_HOME}/share/trick/trickops/requirements.txt`. Additionally, some of the CML model input files require non-system packages like `numpy`. Creating the needed `.venv` directory is easy, simply run the following in a shell:

```bash
# Starting from a shell at the top of the cml repo.
python3 -m venv .venv && source .venv/bin/activate && pip3 install --upgrade pip \
  && pip3 install -r requirements.txt
```
The above steps create the `cml/.venv` directory and also activate it. After it's created, it can be activated in other shells by running:

```bash
# Starting from a shell at the top of the cml repo
source .venv/bin/activate
```
The CML project `.bashrc` automatically sources `.venv/bin/activate`.

## `test.py`

`bin/test.py` is a single script which builds, runs, and compares data for all unit sims that ship with CML. This script expects pre-built `$TRICK_HOME` and `$JEOD_HOME` libraries in the manner described above. Our CI system runs this script and its return code is used to determine success or failure.

When testing locally, we recommend that you clone Trick and JEOD into the `externals/` directory if you don't already have them clone elsewhere. Trick and JEOD are already gitignore'd there.

# Setting up CML as a developer

Configure the repository for development:
1. Clone [cml](https://github.com/nasa/cml) into your desired dev directory
2. Clone [Trick 25.0.2](https://github.com/nasa/trick) or newer into the cml [`externals`](externals) directory.
3. Clone [JEOD 5.0](https://github.com/nasa/jeod) or newer into the cml [`externals`](externals) directory.
4. Setup Python3 virtual environment [.venv](#getting-a-python3-virtual-environment-venv).
5. Optionally, create an [alias](#environment) in your .bashrc for fast setup in the future.

> [!NOTE]
> Setting Trick environment variables such as `TRICK_CFLAGS` and `TRICK_CXXFLAGS`
> may interfere with the build. If you're seeing issues, double check that you're not setting
> Trick environment variables such as these outside of a sims's `S_overrides.mk`.

6. Activate your Python3 virtual environment.
7. [Build Trick, then build and trickify JEOD](#trick-and-jeod).

At this point, you're ready to work in CML.

Build and test CML:

1. Build cml with [cmake](#building-cml)
2. run `bin/test.py` from the top level cml directory

If all tests pass, your dev environment was setup correctly!


# Contact

If after browsing the project wiki you still have questions, please contact:

##### Nino Tarantino
    CACI - CML Maintainer and primary POC
    B16/105
    antonio.tarantino@nasa.gov

##### Daniel Jordan
    NASA/EG2 - CML Task Lead, CI & Infrastructure
    B16/1158
    daniel.d.jordan@nasa.gov

##### Gary Turner
    Odyssey - Architect of Many CML Models
    B16/139
    gary.w.turner@nasa.gov

##### Jason Arnold
    NASA/EG2 - CML Task Lead
    B16/1158
    jason.s.arnold@nasa.gov
