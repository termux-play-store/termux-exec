TMPDIR=$(mktemp -d)

cp tests/readlink-proc-self-exe $TMPDIR

cd $TMPDIR

ACTUAL_PATH_TO_SELF=$(./readlink-proc-self-exe)
EXPECTED_PATH_TO_SELF=$TMPDIR/readlink-proc-self-exe

if [ "$ACTUAL_PATH_TO_SELF" != "$EXPECTED_PATH_TO_SELF" ]; then
  echo "ERROR(1): Expected '$EXPECTED_PATH_TO_SELF', was '$ACTUAL_PATH_TO_SELF'"
  exit 1
fi

ln -s readlink-proc-self-exe symlinked-binary
ACTUAL_PATH_TO_SELF=$(./symlinked-binary)
if [ "$ACTUAL_PATH_TO_SELF" != "$EXPECTED_PATH_TO_SELF" ]; then
  echo "ERROR(2): Expected '$EXPECTED_PATH_TO_SELF', was '$ACTUAL_PATH_TO_SELF'"
  exit 1
fi

ln -s symlinked-binary nested-symlinked-binary
ACTUAL_PATH_TO_SELF=$(./nested-symlinked-binary)
if [ "$ACTUAL_PATH_TO_SELF" != "$EXPECTED_PATH_TO_SELF" ]; then
  echo "ERROR(3): Expected '$EXPECTED_PATH_TO_SELF', was '$ACTUAL_PATH_TO_SELF'"
  exit 1
fi

cd - > /dev/null

rm -rf $TMPDIR

echo ok
