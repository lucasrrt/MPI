echo "generate matrix"
sh generate/generate.sh
echo "threads"
sh thread/thread
echo "mpi"
sh mpi/mpi.sh
