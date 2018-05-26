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
cp ../generate/matriz .
sh mpi_maquinas.sh
cd ..

echo "mix"
cd mix
cp ../generate/matriz .
sh mix_maquinas.sh
cd ..
