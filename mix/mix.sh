mpic++ mix.cpp -o compute;

echo "mix using 3 nodes"
mpirun -np 3 compute

rm compute
