TMPDIR=$(mktemp -d)

cp tests/realpath $TMPDIR

cd $TMPDIR

ACTUAL_PATH_TO_SELF=$(./realpath /proc/self/exe)
EXPECTED_PATH_TO_SELF=$TMPDIR/realpath

if [ "$ACTUAL_PATH_TO_SELF" != "$EXPECTED_PATH_TO_SELF" ]; then
  echo "ERROR(1): Expected '$EXPECTED_PATH_TO_SELF', was '$ACTUAL_PATH_TO_SELF'"
  exit 1
fi

ACTUAL_PATH_TO_SELF=$(./realpath /$TMPDIR)
EXPECTED_PATH_TO_SELF=$TMPDIR

if [ "$ACTUAL_PATH_TO_SELF" != "$EXPECTED_PATH_TO_SELF" ]; then
  echo "ERROR(1): Expected '$EXPECTED_PATH_TO_SELF', was '$ACTUAL_PATH_TO_SELF'"
  exit 1
fi

cd - > /dev/null

rm -rf $TMPDIR

echo ok
