#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>

using namespace std;

void wahey(int from){
	cout << endl << endl << "WAHEY from " << from << endl << endl;
}

void printArray(int size, double *array){
	int i;
	for(i=0;i < size;i++){
		cout << array[i] << " ";
	}
	cout << endl;
}

int main(int argc, char *argv[]) {
	int proc_number, world_rank;
	int i,j;


	double partial_sum, global_sum;
	partial_sum = global_sum = 0;

	//Initialize the MPI environment
	MPI_Init(&argc, &argv);

	//Get the number of processes
	MPI_Comm_size(MPI_COMM_WORLD, &proc_number);

	//Get the rank of the process
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	//Get the name of the process
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	int name_len;
	MPI_Get_processor_name(processor_name, &name_len);


	//Matrix goes here
	int n = 16000;

	double **buffer = new double*[n];
	double *diagonal = new double[n];
	for(i = 0; i < n; i++)
		buffer[i] = new double[n];

	long int values_size = n*n/(proc_number-1); //this 1 is the master node
	int rows_number = n/2;
	int columns_number = n/((proc_number-1)/2);

	//variables for the time
	struct timeval stop, start;
	double start_seconds, stop_seconds;

	//This is the master node
	if (world_rank == 0) {
		cout << "Size of matrix is " << n << "x" << n << endl;

		//allocating memory for the matrix
		//reading the matrix from the file


		//Constructing Matrix
		for(i=0; i < n; i++){
			for(j=0; j < n; j++)
				buffer[i][j] = 1;
			diagonal[i] = buffer[i][i];
		}

		cout << "matrix: " << endl;
		printArray(5, buffer[0]);
		cout << "diagonal: " << endl;
		printArray(5, diagonal);

		//Starting the time after reading the file
		gettimeofday(&start, NULL);
		start_seconds = start.tv_sec + start.tv_usec*1e-6;

		//Sending the diagonal of the matrix
		int number;
		for(number=0; number<proc_number - 1; number++){
			int x = ((number) % 2) * (n / 2);
			int y = ((number) > 1) * (n / 2);
			MPI_Send(diagonal+x,n/2,MPI_DOUBLE,number+1 ,0, MPI_COMM_WORLD);
			cout << "Diagonal sendo enviada para " << number+1 << endl;
			printArray (5, diagonal + x);
			for(i = x; i < x + n/2; i++){
				MPI_Send(buffer[i]+y, columns_number, MPI_DOUBLE, number+1,0,MPI_COMM_WORLD);
			}
		}

	} else { //These are the others nodes
		int x = ((world_rank - 1) % 2) * (n / 2);
		int y = ((world_rank - 1) > 1) * (n / 2);

		double *received_diagonal = new double[n/2];
		MPI_Recv(received_diagonal, n/2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		cout << "Diagonal recebida por " << world_rank << endl;
		printArray(5,received_diagonal);

		double **received_matrix = new double*[n/2];
		for(i=0;i<n/2;i++)
			received_matrix[i] = new double[columns_number];
		for(i = 0; i < rows_number; i++){
			MPI_Recv(received_matrix[i], columns_number, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		}

		cout << "MAtrix recebida por " << world_rank << endl;
		printArray(5,received_matrix[0]);

		//Calculating the partial_sum
		for(i = 0; i < n/2; i++)
			for(j = 0; j < columns_number; j++){
				partial_sum += received_matrix[i][j] * received_diagonal[i];
			}
		cout << "Soma parcial de " << world_rank << ": " << partial_sum << endl;
	}

	//Using MPI_Reduce to sum the partial_sum of each node
	MPI_Reduce(&partial_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	gettimeofday(&stop, NULL);
	stop_seconds = stop.tv_sec + stop.tv_usec*1e-6;

	if(world_rank == 0){
		cout << "Soma global: " << global_sum << endl;
		printf("Soma global: %f\n", global_sum);
		cout << "Duração: " << stop_seconds - start_seconds << endl;
	}

	//Finalize the MPI environment
	MPI_Finalize();
}
