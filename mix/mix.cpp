#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <sys/time.h>
#include <pthread.h>

#define MAX 32

using namespace std;

struct compute_params{
	int *buffer, size;
	int index;
	int i1,i2,j1,j2;
	long int *result;
	pthread_t tid;
};
long int partials[MAX];

void *partial_compute(void *args){
	struct compute_params *params = (struct compute_params*) args;
	partials[params->index] = 0;

	int i, j;
	for(i=params->i1;i<params->i2;i++){
		int diagonal = params->buffer[i*params->size+i];
		for(j=params->j1;j<params->j2;j++){
			int cell = params->buffer[i*params->size+j]*diagonal;
			partials[params->index]+=cell;
		}
	}

	pthread_exit(partials+params->index);
}

int dualthread_sum_elements(int *buffer, int n){
	struct compute_params *p0 = (struct compute_params *)malloc(sizeof(struct compute_params));
	p0->buffer = buffer;
	p0->size = n;
	p0->i1 = 0;
	p0->i2 = n/2;
	p0->j1 = 0;
	p0->j2 = n/2;
	p0->index = 0;
	pthread_create(&p0->tid, NULL, partial_compute, (void *)p0);

	struct compute_params *p1 = (struct compute_params *)malloc(sizeof(struct compute_params));
	p1->buffer = buffer;
	p1->size = n;
	p1->i1 = 0;
	p1->i2 = n/2;
	p1->j1 = n/2;
	p1->j2 = n;
	p1->index = 1;
	pthread_create(&p1->tid, NULL, partial_compute, (void *)p1);
	long int *s0;
	long int *s1;
	pthread_join(p0->tid,(void **) &s0);
	pthread_join(p1->tid,(void **) &s1);

	return partials[0]+partials[1];
}


int main(int argc, char *argv[]) {
	int proc_number, world_rank;
	int i,j;

	long long unsigned int partial_sum, global_sum;
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
	int n = 16000;
	int *buffer = (int*)malloc(sizeof(int)*(n*n));

	long int values_size = n*n/(proc_number-1); //this 1 is the master node
	int rows_number = n/2;
	int columns_number = n/((proc_number-1)/2);


	//Starting the timer
	struct timeval stop, start;
	double start_seconds, stop_seconds;

	//This is the master node
	if (world_rank == 0) {
		cout << "Size of matrix is " << n << "x" << n << endl;

		//allocating memory for the matrix
		for(i=0; i < n; i++){
			for(j=0; j < n; j++)
				buffer[n*i+j] = 1;
		}

		gettimeofday(&start, NULL);
		start_seconds = start.tv_sec + start.tv_usec*1e-6;

		int number;
		for(number=0; number<proc_number - 1; number++){
			int x = ((number) % 2) * n*(n / 2);
			MPI_Send(buffer+x, values_size, MPI_INT, number+1,0,MPI_COMM_WORLD);
		}
	} else { //These are the others nodes
		int *received_values = new int[values_size];
		MPI_Recv(received_values, values_size, MPI_INT, 0, 0, MPI_COMM_WORLD,MPI_STATUS_IGNORE);

		partial_sum = dualthread_sum_elements(received_values,n);
	}

	//Using MPI_Reduce to sum the partial_sum of each node
	MPI_Reduce(&partial_sum, &global_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	gettimeofday(&stop, NULL);
	stop_seconds = stop.tv_sec + stop.tv_usec*1e-6;
	if(world_rank == 0){
		cout << "Soma global: " << global_sum << endl;
		cout << "Duração: " << stop_seconds - start_seconds << endl;
	}

	//Finalize the MPI environment
	MPI_Finalize();
}
