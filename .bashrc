export CML_HOME="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
export CTEST_PARALLEL_LEVEL=10

venv="${CML_HOME}/.venv/bin/activate"
if [ -f "$venv" ]; then
    source "$venv"
else
    echo "WARNING: Cannot find $venv!"
    echo "CML Virtual Environment setup is incomplete. See bin/README.md."
fi

if [[ -z "${JEOD_HOME}" ]]; then
  echo "WARNING: Environment variable JEOD_HOME isn't defined. See README.md"
elif [[ ! -d "${JEOD_HOME}" ]]; then
  echo "WARNING: Environment variable JEOD_HOME's path $JEOD_HOME doesn't exist. See README.md"
else
  if [[ "$(basename $JEOD_HOME)" != 'jeod' ]]; then
    echo "WARNING: JEOD_HOME's basename is not 'jeod' which may prevent some"
    echo "CML models from finding #include paths when compiling. This can be"
    echo "resolved by using a symbolic link and pointing JEOD_HOME to it:" 
    echo "  ln -s $JEOD_HOME $CML_HOME/jeod"
    echo "  export JEOD_HOME=$CML_HOME/jeod"
    echo "See the README.md in CML for more information on why this is needed."
  fi

fi
if [[ -z "${TRICK_HOME}" ]]; then
  echo "WARNING: Environment variable TRICK_HOME isn't defined."
fi

