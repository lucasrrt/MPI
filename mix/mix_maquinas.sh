mpic++ mix.cpp -o compute;

echo "mix using 3 nodes"
mpirun -np 3 -machinefile ./maquinas compute

rm compute
