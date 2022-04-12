#include <stdio.h>
#include <pthread/pthread.h>
#include <unistd.h>
#define NTHREADS 1000

int count = 0;

void *threadTask(void *_) {
    count = count + 1;
    return NULL;
}

int main(int argc, const char * argv[]) {
    pthread_t t[NTHREADS];
    int iter = 0;
    while(1) {
        count = 0;
        iter++;
        for (int i = 0; i < NTHREADS; i++) {
            pthread_create(t+i, NULL, threadTask, NULL);
        }
        for (int i = 0; i < NTHREADS; i++) {
            pthread_join(t[i], NULL);
        }
        if(count != NTHREADS) {
            printf("Got %d at %d iter\n", count, iter);
            break;
        }
    }
    return 0;
}
