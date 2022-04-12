#include <stdio.h>
#include <stdlib.h>
#include <pthread/pthread.h>
#include <time.h>
#include <unistd.h>

void *measureUnlock(void *);

int main(int argc, const char * argv[]) {
    pthread_t timeToUnlock;
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_lock(&mutex);
    pthread_create(&timeToUnlock, NULL, measureUnlock, &mutex);
    puts("Created thread, press any key to unlock");
    getchar();
    pthread_mutex_unlock(&mutex);
    pthread_join(timeToUnlock, NULL);
    return 0;
}

void *measureUnlock(void * m) {
    time_t init = time(NULL);
    pthread_mutex_lock(m);
    time_t after = time(NULL);
    pthread_mutex_unlock(m);
    printf("Took %ld seconds to unlock\n", after - init);
    fflush(stdout);
    return NULL;
}
