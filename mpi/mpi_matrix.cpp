#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>

using namespace std;

void printArray(int size, int *array){
	for(int i = 0; i < size; i++)
		cout << array[i] << " ";
	cout << endl;
}

int main(int argc, char *argv[]) {
	int proc_number, world_rank;
	
	int partial_sum, global_sum;
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
	ifstream file("matrix");
	file >> n;
	int values_size = n*n/(proc_number-1); //this 1 is the master node

	//This is the master node
	if (world_rank == 0) {
		cout << "Size of matrix is " << n << "x" << n << endl;
	
		//allocating memory for the matrix
		int **buffer = new int*[n];
		for (int i = 0; i < n; i++)
			buffer[i] = new int[n];
		//reading the matrix from the file
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
		cout << endl;

		//Sending the diagonal of the matrix
		int *superior_diagonal = new int[n/2];
		int *inferior_diagonal = new int[n/2];
		for(int i = 0; i < n; i++){
			if (i < n/2)
				superior_diagonal[i] = buffer[i][i];
			else
				inferior_diagonal[i - n/2] = buffer[i][i];
		}

		cout << "Diagonais: " << endl << "Superior: ";
		printArray(n/2, superior_diagonal);

		cout << "Inferior: ";
		printArray(n/2, inferior_diagonal);

		cout << endl << endl;

		if(proc_number == 3){
			MPI_Send(superior_diagonal, n/2, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_INT, 2, 0, MPI_COMM_WORLD);
		} else if (proc_number == 5){
			MPI_Send(superior_diagonal, n/2, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(superior_diagonal, n/2, MPI_INT, 2, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_INT, 3, 0, MPI_COMM_WORLD);
			MPI_Send(inferior_diagonal, n/2, MPI_INT, 4, 0, MPI_COMM_WORLD);
		} else {
			cout << "Número inválido de processadores." << endl;
			return 1;
		}

		//spliting the matrix to send values
		//the values to send will be a vector which the 
		//first value is the size of matrix, and the other
		//values will be the respective values

		if(proc_number == 3){
			int *values_to_send1 = new int[values_size];
			int *values_to_send2 = new int[values_size];

			for (int i = 0; i < n/2; i++){
				for (int j = 0; j < n; j++){
					values_to_send1[n*i+j] = buffer[i][j];
					values_to_send2[n*i+j] = buffer[i+n/2][j];
				}
			}

			cout << endl << "valores para mandar para 1:" << endl;
			printArray(values_size, values_to_send1);
			cout << endl << endl;

			cout << "valores para mandar para 2:" << endl; 
			printArray(values_size, values_to_send2);
			cout << endl << endl;

			MPI_Send(values_to_send1, values_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send2, values_size, MPI_INT, 2, 0, MPI_COMM_WORLD);
		} else if (proc_number == 5){
			int *values_to_send1 = new int[values_size];
			int *values_to_send2 = new int[values_size];
			int *values_to_send3 = new int[values_size];
			int *values_to_send4 = new int[values_size];

			for (int i = 0; i < n/2; i++){
				for (int j = 0; j < n/2; j++){
					values_to_send1[n*i/2+j] = buffer[i][j];
					values_to_send2[n*i/2+j] = buffer[i][j+n/2];
					values_to_send3[n*i/2+j] = buffer[i+n/2][j];
					values_to_send4[n*i/2+j] = buffer[i+n/2][j+n/2];
				}
			}

			cout << "valores para mandar para 1:" << endl;
			printArray(values_size, values_to_send1);
			cout << endl << endl;

			cout << "valores para mandar para 2:" << endl; 
			printArray(values_size, values_to_send2);
			cout << endl << endl;
			
			cout << "valores para mandar para 3:" << endl; 
			printArray(values_size, values_to_send3);
			cout << endl << endl;
			
			cout << "valores para mandar para 4:" << endl; 
			printArray(values_size, values_to_send4);
			cout << endl << endl;

			MPI_Send(values_to_send1, values_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send2, values_size, MPI_INT, 2, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send3, values_size, MPI_INT, 3, 0, MPI_COMM_WORLD);
			MPI_Send(values_to_send4, values_size, MPI_INT, 4, 0, MPI_COMM_WORLD);

		}

	} else {
		int received_diagonal[n/2];
		MPI_Recv(received_diagonal, n/2, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		cout << "Diagonal recebida por " << world_rank << ": ";
		printArray(n/2, received_diagonal);

		int received_values[values_size];
		MPI_Recv(received_values, values_size, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);
		cout << "Valores recebidos por " << world_rank << ": ";
		printArray(values_size, received_values);

		cout << endl << endl;

		//Calculating values to retrieve to master node
		int new_values[values_size];
		int columns_number = n/((proc_number-1)/2);
		for(int i = 0; i < n/2; i++)
			for(int j = 0; j < columns_number; j++){
				new_values[i*columns_number + j] = received_values[i*columns_number + j] * received_diagonal[i];
				partial_sum += new_values[i*columns_number+j];
			}

		cout << "Novos valores a serem enviados por " << world_rank << ": ";
		printArray(values_size, new_values);
		cout << "Soma parcial de " << world_rank << ": " << partial_sum << endl;

		cout << endl << endl;
	}
	
	//Using MPI_Reduce to sum the partial_sum of each node
	MPI_Reduce(&partial_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if(world_rank == 0){
		cout << "Soma global: " << global_sum << endl;
	}

	//Finalize the MPI environment
	MPI_Finalize();
}
