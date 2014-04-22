time $PTEST_BINARY $ARGS > test.stdout

cat test.stdout | sort > test.stdout.sort
cat gold.stdout | sort > gold.stdout.sort

diff -u test.stdout.sort gold.stdout.sort
exit $?
