#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

// struct da thread
struct Thread_Data {
	int *arr;
	int len;
	// informações computadas
	int sum;
};

void *thread_task(void *);

int main() {
	// parametros
	int nelem = 1000;
	int nthreads = 6;

	// inicialização do campo a ser somado
	int *arr = malloc(sizeof(int)*nelem);
	for(int i = 0; i < nelem; i++) arr[i] = i+1;
	
	// alocação da estruturas relacionadas as threads
	pthread_t *threads = malloc(sizeof(pthread_t)*nthreads);
	struct Thread_Data *data = malloc(sizeof(struct Thread_Data)*nthreads);

	// inicialização do intervalo que cada thread processa
	int blk = nelem/nthreads;
	int offset = 0;
	for(int i = 0; i < nthreads; i++) {
		data[i].arr = arr+offset;
		data[i].len = blk;
		offset += blk;
	}
	// para não deixar ninguém de fora
	data[nthreads-1].len += nelem%nthreads;

	// cria as thread
	for(int i = 0; i < nthreads; i++) {
		pthread_create(threads+i, NULL, thread_task, data+i);
	}
	
	// da o joint e computa a saida
	int sum = 0;
	for(int i = 0; i < nthreads; i++) {
		pthread_join(threads[i], NULL);
		// depois do join a gente sabe que a thread i
		// ja rodou
		sum += data[i].sum;
	}

	printf("%d\n", sum);
	return 0;
}


void *thread_task(void * _data) {
	struct Thread_Data *data = _data;
	data->sum = 0;
	for(int i = 0; i < data->len; i++)
		data->sum += data->arr[i];
	return NULL;
}
