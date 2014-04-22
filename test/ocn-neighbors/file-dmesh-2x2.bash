#include "tempdir.bash"

cat >gold.stdout <<EOF
"(1, 0)" 0 -> "(0, 0)" 2: 1
"(0, 1)" 3 -> "(0, 0)" 1: 1
"(0, 0)" 2 -> "(1, 0)" 0: 1
"(1, 1)" 3 -> "(1, 0)" 1: 1
"(1, 1)" 0 -> "(0, 1)" 2: 1
"(0, 0)" 1 -> "(0, 1)" 3: 1
"(0, 1)" 2 -> "(1, 1)" 0: 1
"(1, 0)" 1 -> "(1, 1)" 3: 1
EOF

ARGS="--file gold.stdout"

#include "harness.bash"

