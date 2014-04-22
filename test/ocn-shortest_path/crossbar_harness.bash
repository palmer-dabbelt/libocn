#include "tempdir.bash"

ARGS="--crossbar $COUNT"

# Generate the correct output file that is expected to match
for s in $(seq 0 $(($COUNT - 1)))
do
    for d in $(seq 0 $(($COUNT - 1)))
    do
        if [[ "$s" != "$d" ]]
        then
            echo "$s -> $d: 1" >> gold.stdout
        fi
    done
done
cat gold.stdout

#include "harness.bash"

# Check we have the correct number of routes in the graph, which is
# easily computable from a mesh topology.  This shouldn't actually be
# necessary, but it's a nice sanity check for our network generator
# above.
n="$(($WIDTH * $HEIGHT))"
routes="$(( $n * ($n - 1) ))"
if [[ "$(cat test.stdout | wc -l)" != "$routes" ]]
then
    echo "Wrong number of routes"
    echo "  expected $routes"
    echo "  obtained $(cat mesh | wc -l)"
    exit 1
fi
