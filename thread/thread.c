#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

int monothread_sum_elements(int *buffer, int n){
	long int sum = 0;
	for(int i=0;i<n;i++){
		int diagonal = buffer[i*n+i];
		for(int j=0;j<n;j++)
			sum+=buffer[i*n+j]*diagonal;
	}
	return sum;
}

struct compute_params{
	int *buffer, size;
	int upper;
	long int *result;
	pthread_t tid;
};

void *half_compute(void *args){
	struct compute_params *params = (struct compute_params*) args;
	params->result = 0;

	for(int i=params->upper*params->size/2;i<params->upper*params->size;i++){
		int diagonal = params->buffer[i*params->size+i];
		for(int j=0;j<params->size;j++)
			params->result+=params->buffer[i*params->size+j]*diagonal;
	}

	pthread_exit(NULL);

}

int dualthread_sum_elements(int *buffer, int n){
	struct compute_params *upper_params;
	upper_params->buffer = buffer;
	upper_params->size = n;
	upper_params->upper = 1;
	pthread_create(&upper_params->tid, NULL, half_compute, (void *)upper_params);

	struct compute_params *lower_params;
	lower_params->buffer = buffer;
	lower_params->size = n;
	lower_params->upper = 0;
	pthread_create(&lower_params->tid, NULL, half_compute, (void *)lower_params);

	pthread_join(upper_params->tid, NULL);
	pthread_join(lower_params->tid, NULL);

	return *upper_params->result + *lower_params->result;
}

int quadthread_sum_elements(int *buffer, int n){
	long int sum = 0;
	return sum;
}

int main(int argc, char **argv){
	if(argc!=3){
		fprintf(stderr,"Modo de uso: compute FILE THREAD_COUNT");
		return 1;
	}

	int thread_count = atoi(argv[2]);

	int n;
	FILE *file = fopen(argv[1],"r");
	fread(&n,sizeof(int),1,file);

	int *buffer = (int*)malloc(sizeof(int)*(n*n+1));
	fread(buffer,sizeof(int),n*n,file);

	long int sum;
	switch(thread_count){
		case 1:
			sum = monothread_sum_elements(buffer,n);
			break;
		case 2:
			sum = dualthread_sum_elements(buffer,n);
			break;
		case 3:
			sum = quadthread_sum_elements(buffer,n);
			break;
	}

	printf("%ld",sum);
	return 0;
}
