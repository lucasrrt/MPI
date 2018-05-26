
mpic++ mpi.cpp -o compute;

echo "mpi using 3 nodes"
mpirun -np 3 -machinefile ./maquinas compute

echo "mpi using 5 nodes"
mpirun -np 5 -machinefile ./maquinas compute

rm compute
