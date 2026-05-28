#!/bin/bash
# This script performs tests that should always pass, are performed quickly,
# and do not rely on any build mechanisms. "Fail early and often" is the
# goal here. These tests include:
INSTALL_=`readlink -e "$0"`
INSTALL_=`dirname "$INSTALL_"`
cd $INSTALL_/..
TOP=`pwd`

logfile="$TOP/bin/.scripting_tests_out.txt"
touch $logfile

# Execute one test, redirect it to log file, and evaluate success/failure
# Exits immediately on failures with args expected to be
#   $1=string defining test to evaluate
#   $2=expected return code
function test_and_exit_if_failed {
    printf "%-80s" "Testing \"$1\" ..."
    eval $1 >> "$logfile" 2>&1
    ret=$?
    if [ "$ret" != $2 ]; then
        echo " FAILED (Expected: $2, Got: $ret)"
        echo "See $logfile for failure details."
        echo "Run antares/bin/.scripting_tests.sh in your local workspace to replicate"
        echo "Exiting with return code 1."
        exit 1
    fi
    printf " OK\n"
}

cd $TOP

# Check for python 2 syntax usage which is not allowed
test_and_exit_if_failed "git grep '^[[:blank:]]*print \"' -- :antares*.py" 1
test_and_exit_if_failed "git grep '^[[:blank:]]*print\"' -- :antares*.py" 1
test_and_exit_if_failed "git grep \"^[[:blank:]]*print '\" -- :antares*.py" 1
test_and_exit_if_failed "git grep \"^[[:blank:]]*print'\" -- :antares*.py" 1
test_and_exit_if_failed "git grep \"^[[:blank:]]*print [[:alnum:]]\" -- :antares*.py" 1
test_and_exit_if_failed "git grep 'execfile(' -- :antares*.py" 1

# Check for tabs - python3 doesn't allow tabs and spaces to mix, so we disallow tabs
echo "Searching input files for tabs across all of antares/ ..." | tee -a $logfile
echo "If any are found replace tabs with spaces to resolve the issue." | tee -a $logfile
tabs_tempfile=$(mktemp /tmp/.files_containing_tabs.XXXXXX)
# Produce a unique list of all *.py files containing tabs, write to a temp file
git ls-files |grep '\.py$' |xargs -d '\n' grep -P '\t' |awk -F : '{print "Tabs found in: " $1}' | uniq >& $tabs_tempfile
# Assert that the temp file contains nothing, this is our success criteria
test_and_exit_if_failed "grep '.*' $tabs_tempfile" 1

# Produce a unique list of all *.py files containing CRLF line endings , write to a temp file
echo "Searching input files with DOS (CRLF) line endings across all of cml/ ..." | tee -a $logfile
echo "If any are found this check will fail, use 'dos2unix <file>' to resolve the issue." | tee -a $logfile
crlf_tmpfile=$(mktemp /tmp/.DOS_line_endings.XXXXXX)
git ls-files |grep '\.py$\|\.cpp$\|\.cc$\|\.hh$\|\.h$\|\.f$\|\.f90$\|\.tex$|S_define$|\.json$' |xargs file |grep CRLF |awk -F : '{print "DOS (CRLF) line endings found: " $1}' | uniq >& $crlf_tmpfile
# Assert that the temp file contains nothing, this is our success criteria
test_and_exit_if_failed "grep '.*' $crlf_tmpfile" 1

echo "All script testing completed successfully."
exit 0
