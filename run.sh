echo "generate matrix"
cd generate
sh generate.sh
cd ..

echo "threads"
cd thread
sh thread/thread
cd ..

echo "mpi"
cd mpi
sh mpi/mpi_maquinas.sh
cd ..
