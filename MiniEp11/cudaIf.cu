/*
mini EP 11

NOME: Your name here
NUSP: Your NUSP here
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

long getMS() {
    struct timespec s;
    clock_gettime(CLOCK_REALTIME, &s);
    return s.tv_sec*1000 + s.tv_nsec/1000000;
}

// number of tests
#define NTESTS 10
#define SEED 123456
#define SIZE 1024
#define WIDTH 512
#define ROUNDS 1000

long cudaSum(int *);
long cudaIfSum(int *);

int main() {
    srand(SEED);
    long cudaTime = 0;
    long cudaIfTime = 0;

    int * reference = (int *)malloc(sizeof(int)*SIZE*WIDTH);

    for(int i = 0; i < NTESTS; i++) {
        for(int j = 0; j < SIZE*WIDTH; j++) reference[j] = rand()%5096;
        cudaTime += cudaSum(reference);
        cudaIfTime += cudaIfSum(reference);
    }

    free(reference);

    printf("Average cudaTime %ldms\nAvarage cudaIfTime %ldms\n", cudaTime/NTESTS, cudaIfTime/NTESTS);
}

// Conditional Vector Sum

__global__ void cudaIfSumGPU(int *ints) {
    int sum = 0;

    int off = blockIdx.x*32 + threadIdx.x;

    for(int j = 0; j < ROUNDS; j++) {
        for(int i = 0; i < WIDTH; i++) {
            if(ints[WIDTH*off+i] % 2)
                sum += (int) sqrt((double)ints[WIDTH*off+i]);
            else
                sum += (int) sqrt((double)ints[WIDTH*off+i]);
        }

        sum = sum/128;
    }

    ints[WIDTH*SIZE+off] = sum;
}

long cudaIfSum(int *refs) {
    int *cudaRefs;
    int results[SIZE];

    cudaMalloc(&cudaRefs, sizeof(int)*SIZE*(WIDTH+1));
    cudaMemcpy(cudaRefs, refs, sizeof(int)*WIDTH*SIZE, cudaMemcpyHostToDevice);

    long t0 = getMS();
    cudaIfSumGPU<<<32,SIZE/32>>>(cudaRefs);

    cudaMemcpy(results, cudaRefs+(WIDTH*SIZE), sizeof(int)*SIZE, cudaMemcpyDeviceToHost);

    int sum = 0;
    for(int i = 0; i < SIZE; i++)
        sum += results[i];

    long tf = getMS();

    cudaFree(cudaRefs);

    printf("CUDA IF SUM: %d\n", sum);

    return tf-t0;
}

// Non Conditional Vector Sum

__global__ void cudaSumGPU(int *ints) {
    int sum = 0;

    int off = blockIdx.x*32 + threadIdx.x;

    for(int j = 0; j < ROUNDS; j++) {
        for(int i = 0; i < WIDTH; i++) {
            sum += (int) sqrt((double)ints[WIDTH*off+i]);
        }
        sum = sum/128;
    }

    ints[WIDTH*SIZE+off] = sum;
}

long cudaSum(int *refs) {
    int *cudaRefs;
    int results[SIZE];

    cudaMalloc(&cudaRefs, sizeof(int)*SIZE*(WIDTH+1));
    cudaMemcpy(cudaRefs, refs, sizeof(int)*WIDTH*SIZE, cudaMemcpyHostToDevice);

    long t0 = getMS();
    cudaSumGPU<<<32,SIZE/32>>>(cudaRefs);

    cudaMemcpy(results, cudaRefs+(WIDTH*SIZE), sizeof(int)*SIZE, cudaMemcpyDeviceToHost);

    int sum = 0;
    for(int i = 0; i < SIZE; i++)
        sum += results[i];

    long tf = getMS();

    cudaFree(cudaRefs);

    printf("CUDA SUM: %d\n", sum);

    return tf-t0;
}
