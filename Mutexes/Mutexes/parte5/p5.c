#include <stdio.h>
#include <stdlib.h>
#include <pthread/pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

void *consumir(void *);
void *produzir(void *);

int contains = 0;
int end = 0;
int buffer[10];
pthread_mutex_t consume;
pthread_mutex_t insert;

int main(int argc, const char * argv[]) {
    pthread_t produtor;
    pthread_t consumidor;
    pthread_mutex_init(&consume, NULL);
    pthread_mutex_init(&insert, NULL);
    pthread_mutex_lock(&consume);
    pthread_create(&produtor, NULL, produzir, NULL);
    pthread_create(&consumidor, NULL, consumir, NULL);
    pthread_join(consumidor, NULL);
    pthread_join(produtor, NULL);
    pthread_mutex_destroy(&consume);
    return 0;
}

void *consumir(void *_) {
    while(1) {
        pthread_mutex_lock(&consume);
        pthread_mutex_lock(&insert);
	if(contains == 0 && end) return NULL; 
        int c = buffer[--contains];
        printf("Consumindo %d\n", c);
        fflush(stdout);
        if(contains != 0) pthread_mutex_unlock(&consume);
        else if(end) return NULL;
        pthread_mutex_unlock(&insert);
        sleep(1);
    }
    return NULL;
}

void *produzir(void *_) {
    printf("Enter non zero numbers to add to the queue and 0 to end\n");
    int input;
    scanf("%d", &input);
    while(input != 0) {
        pthread_mutex_lock(&insert);
        printf("Adding %d to que task\n", input);
        buffer[contains++] = input;
        if(contains == 1) pthread_mutex_unlock(&consume);
        pthread_mutex_unlock(&insert);
        scanf("%d", &input);
    }
    pthread_mutex_lock(&insert);
    end = 1;
    if(contains == 0) pthread_mutex_unlock(&consume);
    pthread_mutex_unlock(&insert);
    return NULL;
}








// A if(contains == 0 && end) return NULL;
// B else if(end) return NULL;
