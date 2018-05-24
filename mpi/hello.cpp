#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
	int number_of_process, world_rank;

	//if(argc!=3){
	//	fprintf(stderr, "Modo de uso: compute FILE THREAD_COUNT");
	//	return 1;
	//}

	//Initialize the MPI environment
	MPI_Init(&argc, &argv);

	//Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);

	//Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//Get the name of the process
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);

	//Here will go the logic of the program


	int number1;
	int number2;
	int number3;
	int number4;
	int number_received;
	if (world_rank == 0) {
		number1 = 1;
		number2 = 2;
		number3 = 3;
		number4 = 4;
		MPI_Send(&number1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(&number2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		MPI_Send(&number3, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		MPI_Send(&number4, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
	} else if (world_rank == 1) {
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);

	} else if (world_rank == 2){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);

	} else if (world_rank == 3){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);

	} else if (world_rank == 4){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);
		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);
	}



	//Finalize the MPI environment
	MPI_Finalize();
}
