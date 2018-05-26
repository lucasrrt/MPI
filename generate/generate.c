#include<stdio.h>
#include<time.h>
#include<stdlib.h>

int main(int argc, char **argv){
	if(argc!=3){
		fprintf(stderr,"Modo de uso: ./generate N file");
		return 1;
	}
	int n = atoi(argv[1]);
	int* buffer = (int*)malloc(sizeof(int)*n*n);
	srand(time(NULL));

	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			buffer[i*n+j] = 1;
		}
	}

	FILE *file = fopen(argv[2],"wb");
	fwrite(&n,sizeof(int),1,file);
	fwrite(buffer,sizeof(int),n*n,file);
	return 0;
}
