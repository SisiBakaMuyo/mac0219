#include <stdio.h>
#include <pthread/pthread.h>
#define NTHREADS 1000
#define MAXTRIES 200

int count = 0;
pthread_mutex_t mutex;

void *threadTask(void *_) {
    pthread_mutex_lock(&mutex);
    count = count + 1;
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main(int argc, const char * argv[]) {
    pthread_t t[NTHREADS];
    int iter = 0;
    pthread_mutex_init(&mutex, NULL);
    for(int j = 0; j < MAXTRIES; j++) {
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
            goto end;
        }
    }
    puts("Worked!");
end:
    return 0;
}
