#!/bin/bash

set -u

UNAME_OS=$(uname -o)
EXIT_CODE=0

for f in tests/*.sh; do
	if [ "$UNAME_OS" != Android ]; then
		if [ "$f" = tests/call-system-bin-sh.sh ] || [ "$f" = tests/print-argv0.sh ] || [ "$f" = tests/fexecve.sh ] ; then
			echo "Skipping $f..."
			continue
		fi
	fi

	printf "Running $f..."

	EXPECTED_FILE=$f-expected
	ACTUAL_FILE=$f-actual

	rm -f $ACTUAL_FILE
	$f myarg1 myarg2 &> $ACTUAL_FILE

	if cmp --silent $ACTUAL_FILE $EXPECTED_FILE; then
		printf " OK\n"
	else
		printf " FAILED - compare expected ${EXPECTED_FILE} with ${ACTUAL_FILE}\n"
		echo "### Expected:"
		cat "$EXPECTED_FILE"
		echo "### Actual:"
		cat "$ACTUAL_FILE"
		echo "###"
		EXIT_CODE=1
	fi
done

exit $EXIT_CODE
