echo "generate matrix"
cd generate
sh generate.sh
cd ..

echo "threads"
cd thread
sh thread.sh
cd ..

echo "mpi"
cd mpi
sh mpi_maquinas.sh
cd ..

echo "mix"
cd mix
sh mix_maquinas.sh
cd ..
