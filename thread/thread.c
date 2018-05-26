#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<sys/time.h>

#define MAX 32

int monothread_sum_elements(int *buffer, int n){
	long long unsigned int sum = 0;
	for(int i=0;i<n;i++){
		int diagonal = buffer[i*n+i];
		for(int j=0;j<n;j++)
			sum+=buffer[i*n+j]*diagonal;
	}
	return sum;
}

struct compute_params{
	int *buffer, size;
	int index;
	int i1,i2,j1,j2;
	long long unsigned int *result;
	pthread_t tid;
};
long long unsigned int partials[MAX];

void *partial_compute(void *args){
	struct compute_params *params = (struct compute_params*) args;
	partials[params->index] = 0;

	for(int i=params->i1;i<params->i2;i++){
		int diagonal = params->buffer[i*params->size+i];
		for(int j=params->j1;j<params->j2;j++){
			int cell = params->buffer[i*params->size+j]*diagonal;
			partials[params->index]+=cell;
		}
	}

	pthread_exit(partials+params->index);
}

int dualthread_sum_elements(int *buffer, int n){
	struct compute_params *p0 = malloc(sizeof(struct compute_params));
	p0->buffer = buffer;
	p0->size = n;
	p0->index = 0;
	p0->i1 = 0;
	p0->i2 = n/2;
	p0->j1 = 0;
	p0->j2 = n;
	pthread_create(&p0->tid, NULL, partial_compute, (void *)p0);

	struct compute_params *p1 = malloc(sizeof(struct compute_params));
	p1->buffer = buffer;
	p1->size = n;
	p1->index = 1;
	p1->i1 = n/2;
	p1->i2 = n;
	p1->j1 = 0;
	p1->j2 = n;
	pthread_create(&p1->tid, NULL, partial_compute, (void *)p1);

	long long unsigned int *s0;
	long long unsigned int *s1;
	pthread_join(p0->tid,(void **) &s0);
	pthread_join(p1->tid,(void **) &s1);

	return partials[0]+partials[1];
}

int quadthread_sum_elements(int *buffer, int n){
	struct compute_params *p0 = malloc(sizeof(struct compute_params));
	p0->buffer = buffer;
	p0->size = n;
	p0->i1 = 0;
	p0->i2 = n/2;
	p0->j1 = 0;
	p0->j2 = n/2;
	p0->index = 0;
	pthread_create(&p0->tid, NULL, partial_compute, (void *)p0);

	struct compute_params *p1 = malloc(sizeof(struct compute_params));
	p1->buffer = buffer;
	p1->size = n;
	p1->i1 = 0;
	p1->i2 = n/2;
	p1->j1 = n/2;
	p1->j2 = n;
	p1->index = 1;
	pthread_create(&p1->tid, NULL, partial_compute, (void *)p1);

	struct compute_params *p2 = malloc(sizeof(struct compute_params));
	p2->buffer = buffer;
	p2->size = n;
	p2->i1 = n/2;
	p2->i2 = n;
	p2->j1 = 0;
	p2->j2 = n/2;
	p2->index = 2;
	pthread_create(&p2->tid, NULL, partial_compute, (void *)p2);

	struct compute_params *p3 = malloc(sizeof(struct compute_params));
	p3->buffer = buffer;
	p3->size = n;
	p3->i1 = n/2;
	p3->i2 = n;
	p3->j1 = n/2;
	p3->j2 = n;
	p3->index = 3;
	pthread_create(&p3->tid, NULL, partial_compute, (void *)p3);

	long long unsigned int *s0;
	long long unsigned int *s1;
	long long unsigned int *s2;
	long long unsigned int *s3;
	pthread_join(p0->tid,(void **) &s0);
	pthread_join(p1->tid,(void **) &s1);
	pthread_join(p2->tid,(void **) &s2);
	pthread_join(p3->tid,(void **) &s3);

	return partials[0]+partials[1]+partials[2]+partials[3];
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

	long long unsigned int sum;
	struct timeval stop, start;
	gettimeofday(&start, NULL);
	switch(thread_count){
		case 1:
			sum = monothread_sum_elements(buffer,n);
			break;
		case 2:
			sum = dualthread_sum_elements(buffer,n);
			break;
		case 4:
			sum = quadthread_sum_elements(buffer,n);
			break;
	}
	gettimeofday(&stop, NULL);

	printf("duration: %lu\n", stop.tv_usec - start.tv_usec);
	printf("sum: %ld\n",sum);
	return 0;
}
