# The `bin/` directory

This directory contains testing infrastructure for the CML project. None of the content here is needed for
CML-model-using groups, it is only for those contributing to the CML repository and testing the models therein.

# Setting up Your Environment

These scripts require specific Python packages to be installed via a virtual environment. Some of them also require
specific environment variables to be set. To get your CML environment set up to use these developer tools, see the
[Getting Started Guide](https://nasa.github.io/cml/contributing/getting-started.html).

# Scripts

## `test.py`

This is a single script which builds, runs, and compares data for all unit sims that ship with CML. Unit sims, runs,
and verification data are defined in the `default.yml` file.

## `apply_cml_common_trick_header.py`

Similar to `clang-format` but specifically for Trick headers in model source code. Applies a common stylesheet to Trick
headers or checks that existing Trick headers conform to the stylesheet used by CML.
