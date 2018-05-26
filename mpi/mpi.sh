$HOME/opt/usr/local/bin/mpic++ mpi_matrix.cpp -o compute;

echo "mpi using 3 nodes"
$HOME/opt/usr/local/bin/mpirun -np 3 compute

echo "mpi using 5 nodes"
$HOME/opt/usr/local/bin/mpirun -np 5 compute
