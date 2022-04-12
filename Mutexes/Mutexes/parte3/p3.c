#include <stdio.h>
#include <stdlib.h>
#include <pthread/pthread.h>
#include <unistd.h>

enum Bolo {
    Aguardando,
    PraparandoMassa,
    MassaPreparada,
    NoForno,
    Assado
};

void prepara(enum Bolo*);
void assa(enum Bolo*);

void roda(int _nbolos, int preparadores, int assadores);

int main(int argc, const char * argv[]) {
    roda(5,    1,  1);
    roda(10,   2,  4);
    roda(100, 10, 20);
    return 0;
}

void prepara(enum Bolo *b) {
    *b = PraparandoMassa;
    sleep(rand()%3+1);
    *b = MassaPreparada;
}

void assa(enum Bolo *b) {
    *b = NoForno;
    sleep(rand()%4+1);
    *b = Assado;
}

int nbolos;
enum Bolo *bolos;

void *preparador(void *_) {
    long preparados = 0;
    for(int i = 0; i < nbolos; i++) {
        if(bolos[i] == Aguardando) {
            prepara(bolos+i);
            preparados++;
        }
    }
    return (void *) preparados;
}

void *assador(void *_) {
    long assados = 0;
    for(int i = 0; i < nbolos; i++) {
        while(bolos[i] < MassaPreparada); // spin lock
        if(bolos[i] == MassaPreparada) {
            assa(bolos+i);
            assados++;
        }
    }
    return (void *) assados;
}

void roda(int _nbolos, int preparadores, int assadores) {
    enum Bolo _bolos[_nbolos];
    nbolos = _nbolos;
    bolos = _bolos;
    for(int i = 0; i < nbolos; i++) bolos[i] = Aguardando;
    
    pthread_t t_prep[preparadores];
    for(int i = 0; i < preparadores; i++)
        pthread_create(t_prep+i, NULL, preparador, NULL);
    
    pthread_t t_assa[assadores];
    for(int i = 0; i < assadores; i++)
        pthread_create(t_assa+i, NULL, assador, NULL);
    
    long preparados = 0;
    long assados = 0;
    void *t;
    
    for(int i = 0; i < preparadores; i++) {
        pthread_join(t_prep[i], &t);
        preparados += (long)t;
    }
        
    
    for(int i = 0; i < assadores; i++) {
        pthread_join(t_assa[i], &t);
        assados += (long)t;
    }
        
    printf("De %d, %ld bolos preparados, %ld bolos assados\n", _nbolos, preparados, assados);
}
