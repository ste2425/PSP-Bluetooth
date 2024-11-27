#!/bin/bash

# https://stackoverflow.com/a/4774063
SCRIPTS_DIR="$( cd -- "$(dirname "$0")" >/dev/null 2>&1 ; pwd -P )"

$SCRIPTS_DIR/compile.sh

$SCRIPTS_DIR/merge.sh