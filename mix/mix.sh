mpic++ mix.cpp -o compute;

echo "mpi using 3 nodes"
mpirun -np 3 compute

echo "mpi using 5 nodes"
mpirun -np 5 compute

rm compute
