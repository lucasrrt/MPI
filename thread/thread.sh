echo "compiling thread.c"
cc ./thread.c -o thread -lpthread
echo "1 thread"
./thread ../generate/matriz 1 > t1.out
echo "2 threads"
./thread ../generate/matriz 2 > t2.out
echo "4 threads"
./thread ../generate/matriz 4 > t4.out
