#include "tempdir.bash"

ARGS="--mesh $WIDTH $HEIGHT"

# Generate the correct output file that is expected to match
for sx in $(seq 0 $(($WIDTH - 1)))
do
    for sy in $(seq 0 $(($HEIGHT - 1)))
    do
        for dx in $(seq 0 $(($WIDTH - 1)))
        do
            for dy in $(seq 0 $(($HEIGHT - 1)))
            do
                if [[ "$sx" != "$dx" || "$sy" != "$dy" ]]
                then
                    xd=$(($sx - $dx))
                    yd=$(($sy - $dy))
                    xda="$(echo "$xd" | awk ' { if($1>=0) { print $1} else {print $1*-1 }}')"
                    yda="$(echo "$yd" | awk ' { if($1>=0) { print $1} else {print $1*-1 }}')"

                    cost=$(($xda + $yda))
                    echo "\"($sx, $sy)\" -> \"($dx, $dy)\": $cost" >> gold.stdout
                fi
            done
        done
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
