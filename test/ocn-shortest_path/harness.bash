$PTEST_BINARY $ARGS > test.stdout

diff -u test.stdout gold.stdout
exit $?

