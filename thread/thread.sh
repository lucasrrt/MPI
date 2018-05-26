echo "compiling thread.c"
cc ./thread.c -o thread -lpthread
echo "1 thread"
./thread ../generate/matriz 1
echo "2 threads"
./thread ../generate/matriz 2
echo "4 threads"
./thread ../generate/matriz 4
