#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {
	int number_of_process, world_rank;


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
	int number_to_send;

	//This is the master node
	if (world_rank == 0) {
		//Reading matrix from file
		int n;
		ifstream file("matrix");
		file >> n;
		cout << "Size of matrix is " << n << "x" << n << endl;

		//int *buffer = (int*)malloc(sizeof(int)*(n*n+1));
		int **buffer = new int*[n];
		for (int i = 0; i < n; i++)
			buffer[i] = new int[n];

		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				file >> buffer[i][j];
			}
		}

		cout << "Matrix:" << endl;
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++)
				cout << buffer[i][j] << " ";
			cout << endl;
		}

		//spliting the matrix to send values
		//the values to send will be a vector which the 
		//first value is the size of matrix, and the other
		//values will be the respective values
		int *values_to_send1 = new int[n*n/2+1];
		int *values_to_send2 = new int[n*n/2+1];
		values_to_send1[0] = n;
		values_to_send2[0] = n;

		for (int i = 0; i < n/2; i++){
			for (int j = 0; j < n; j++){
				values_to_send1[n*i+j+1] = buffer[i][j];
				values_to_send2[n*i+j+1] = buffer[i+n/2][j];
			}
		}


		//Sending info to each process
		MPI_Send(values_to_send1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
		MPI_Send(values_to_send2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
		//MPI_Send(&number3, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
		//MPI_Send(&number4, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);

		//Receiving info from the processes
		MPI_Recv(&number1, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&number2, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//MPI_Recv(&number3, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//MPI_Recv(&number4, 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//Analysing received data
		printf("New number 1 is %d\n", number1);
		printf("New number 2 is %d\n", number2);
		//printf("New number 3 is %d\n", number3);
		//printf("New number 4 is %d\n", number4);

	} else if (world_rank == 1) {
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);

		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);
		number_to_send = number_received*10;

		MPI_Send(&number_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	} else if (world_rank == 2){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);

		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);
		number_to_send = number_received*10;

		MPI_Send(&number_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	} else if (world_rank == 3){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);

		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);
		number_to_send = number_received*10;

		MPI_Send(&number_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	} else if (world_rank == 4){
		MPI_Recv(&number_received, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				MPI_STATUS_IGNORE);

		printf("Process %d received number %d from process 0\n",
				world_rank, number_received);
		number_to_send = number_received*10;
		
		MPI_Send(&number_to_send, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}



	//Finalize the MPI environment
	MPI_Finalize();
}
