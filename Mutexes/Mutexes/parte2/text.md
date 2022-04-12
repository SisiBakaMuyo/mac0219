# O que é um mutex

É um tipo mecanismo de sincronização

## Propriedades

Só possui 2 estados possíveis:
- locked
- unlocked

E 2 operações básicas:
- lock    (L)
- unlock  (U)

## Clássica zona crítica

Thread 1 -L--------U-------->
Thread 2 ----L.....-------U->

## Produtor Consumidor
(C -> consome)
(A -> Adiciona)

Main       -AAAA----AA
Thread 1   --C---C....
Thread 2   --C--C...--
Thread 3   ---C--C...-
Thread 4   ----C--C...
Adicionado 01234444456
Consumido  00234444456
