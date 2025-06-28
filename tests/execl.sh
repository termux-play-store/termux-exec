#!/bin/sh
set -e -u

# Make TMPFILE an existing but non-executable file:
TMPFILE=$(mktemp)
echo "1" > $TMPFILE

./tests/execl $TMPFILE

rm $TMPFILE
