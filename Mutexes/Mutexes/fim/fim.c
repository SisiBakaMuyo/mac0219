#include <stdio.h>
#include <stdlib.h>
#include <pthread/pthread.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>

int tiros = 5;
pthread_mutex_t lock;
void *game(void *);

int main(int argc, const char * argv[]) {
    pthread_t inimigo;
    pthread_mutex_init(&lock, NULL);
    pthread_create(&inimigo, NULL, game, NULL);
    puts("Começou!");
    while(tiros > 0) {
        getchar();
        tiros--;
        puts("Pah!");
        if(pthread_mutex_trylock(&lock) == 0) {
            puts("Você acertou");
            goto end;
        } else {
            printf("Errou, restam %d balas\n", tiros);
        }
        fflush(stdout);
    }
    puts("Ele te atacou e você morreu!");
end:
    return 0;
}


void *game(void *_) {
    pthread_mutex_lock(&lock);
    sleep(10);
    pthread_mutex_unlock(&lock);
    sleep(1);
    return NULL;
}
