# The `bin/` directory

This directory contains testing infrastructure for the CML project. None of the content here is needed for CML-model-using groups, it is only for those contributing to the CML repository and testing the models therein.

# Getting a python3 virtual environment (.venv)

The scripts which manage testing CML content leverage TrickOps modules which themselves require a valid `python3` virtual environment to be created and activated with content defined by `trick/share/trick/trickops/requirements.txt`. Additionally, some of the CML model input files require non-system packages like `numpy`. Creating the needed `.venv` directory is easy, simply run the following:

```bash
# Starting from a shell at the top of the cml repo, with visiblility
# to a Trick directory running version 19.5.1 or later...
python3 -m venv .venv && source .venv/bin/activate && pip3 install --upgrade pip \
  && pip3 install -r requirements.txt
```
The above steps create the `cml/.venv` directory and also activate it. After it's created, it can be activated in other shells by running:

```bash
# Starting from a shell at the top of the cml repo
source .venv/bin/activate
```

# `test.py`

`bin/test.py` is a single script which builds, runs, and compares data for all unit sims that ship with CML. This script expects the following to exist:

1. A pre-built `trick` directory should be read-accessible
2. A `jeod` directory should be read-accessible
