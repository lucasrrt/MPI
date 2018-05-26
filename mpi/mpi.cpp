#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>

using namespace std;

int main(int argc, char *argv[]) {
	int proc_number, world_rank;

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

	//Reading size of  matrix from file
	int n;
	FILE *file = fopen("matrix","rb");
	fread(&n,sizeof(double),1,file);

	long int values_size = n*n/(proc_number-1); //this 1 is the master node

	//variables for the time
	struct timeval stop, start;
	double start_seconds, stop_seconds;

	//This is the master node
	if (world_rank == 0) {
		cout << "Size of matrix is " << n << "x" << n << endl;

		//allocating memory for the matrix
		//reading the matrix from the file
		double *buffer = (double*)malloc(sizeof(double)*(n*n+1));
		fread(buffer,sizeof(double),n*n,file);
	
		//Starting the time after reading the file
		gettimeofday(&start, NULL);
		start_seconds = start.tv_sec + start.tv_usec*1e-6;

		//Sending the diagonal of the matrix
		double *superior_diagonal = new double[n/2];
		double *inferior_diagonal = new double[n/2];
		int i = 0;
		for(i = 0; i < n; i++){
			if (i < n/2)
				superior_diagonal[i] = buffer[n*i+i];
			else
				inferior_diagonal[i - n/2] = buffer[n*i+i];
		}

		if(proc_number == 3){
			MPI_Send(superior_diagonal, n/2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
		} else if (proc_number == 5){
			MPI_Send(superior_diagonal, n/2, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Send(superior_diagonal, n/2, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD);
		} else {
			cout << "Número inválido de processadores." << endl;
			return 1;
		}

		//Splitting and sending each part of the matrix
		if(proc_number == 3){
			double *values_to_send1 = buffer;
			double *values_to_send2 = buffer + values_size;

			MPI_Send(values_to_send1, values_size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send2, values_size, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
		} else if (proc_number == 5){
			double *values_to_send1 = new double[values_size];
			double *values_to_send2 = new double[values_size];
			double *values_to_send3 = new double[values_size];
			double *values_to_send4 = new double[values_size];

			int i, j;
			for (i = 0; i < n/2; i++){
				for (j = 0; j < n/2; j++){
					values_to_send1[n*i/2+j] = buffer[i*n/2 + j];
					values_to_send2[n*i/2+j] = buffer[i*n/2 + j+n/2];
					values_to_send3[n*i/2+j] = buffer[(i+n/2)*n/2 + j];
					values_to_send4[n*i/2+j] = buffer[(i+n/2)*n/2 + j+n/2];
				}
			}

			MPI_Send(values_to_send1, values_size, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send2, values_size, MPI_DOUBLE, 2, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send3, values_size, MPI_DOUBLE, 3, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send4, values_size, MPI_DOUBLE, 4, 0, MPI_COMM_WORLD);
		}
	} else { //These are the others nodes
		double received_diagonal[n/2];
		MPI_Recv(received_diagonal, n/2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		double received_values[values_size];
		MPI_Recv(received_values, values_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		//Calculating the partial_sum
		double new_values[values_size];
		int columns_number = n/((proc_number-1)/2);
		int i,j;
		for(i = 0; i < n/2; i++)
			for(j = 0; j < columns_number; j++){
				new_values[i*columns_number + j] = received_values[i*columns_number + j] * received_diagonal[i];
				partial_sum += new_values[i*columns_number+j];
			}
	}

	//Using MPI_Reduce to sum the partial_sum of each node
	MPI_Reduce(&partial_sum, &global_sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	gettimeofday(&stop, NULL);
	stop_seconds = stop.tv_sec + stop.tv_usec*1e-6;

	if(world_rank == 0){
		cout << "Soma global: " << global_sum << endl;
		cout << "Duração: " << stop_seconds - start_seconds << endl;
	}

	//Finalize the MPI environment
	MPI_Finalize();
}
