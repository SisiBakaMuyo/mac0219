#include <stdio.h>
#include <stdlib.h>

void inicializaVetor(int *v, int tam){
  for (int i = 0; i < tam; i++)
    v[i] = rand() % 100;
}

void imprimeVetor(int *v, int tam){
  for (int i = 0; i < tam; i++)
    printf("v[%d] = %d  ",i, v[i]);
  printf("\n");
}



void vezes2Vetor(int *v, int tam){
  for (int i = 0; i < tam; i++)
    *v++ = *v * 2;
}

int main()
{
  int x[1];
  int vetor[10];
  int v[1];

  x[0] = 1;
  v[0] = 2;
  inicializaVetor(vetor, 10);
  imprimeVetor(vetor, 11);
  vezes2Vetor(vetor,11);
  imprimeVetor(vetor, 11);
  printf("x= %d    v = %d\n",x[0] , v[0]);
  return 0;
 }
