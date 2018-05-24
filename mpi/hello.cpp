#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int npes, myrank;

	//Initialize the MPI environment
	MPI_Init(&argc, &argv);

	//Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &npes);

	//Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

	//Get the name of the process
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	//Print off a Hello world message
	printf("From process %d (%s) out of %d, Hello World!\n", myrank, processor_name, npes);

	//Finalize the MPI environment
	MPI_Finalize();
}
