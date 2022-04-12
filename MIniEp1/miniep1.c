/* miniEP1
 * NOme: Ísis Ardisson Logullo
 * nUSP: 7577410
 * gcc -Wall -ansi -o MiniEP1 miniep1.c -lm
 */

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int main (){
    int n = 1;
    int i, j, k = 0;
    int bitshift = 0;
    int primos = 0;
    int primosEspeciais = 0;
    

    printf("Digite um numero: \n");
    scanf("%d", &bitshift);
    n = n * pow(2,bitshift);
    
    int* crivo= malloc((n+1)*sizeof(int));

    for (i = 2; i < n+1; i++){
        crivo[i] = 1;
    }
    for (i = 2; i < n+1; i++){
        k = i;
        if (crivo[i] == 1){
            primos++;
            if (i % 4 != 3){
                primosEspeciais++;
            }
            for (j = i*2; j < n; k++){
                crivo[j] = 0;
                j = j + k;
            }
        }
    }

    printf("%d %d\n", primos, primosEspeciais);

    return 0;
}

