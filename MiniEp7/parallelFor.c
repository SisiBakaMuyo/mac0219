// Nome: Ísis Ardisson Logullo
// NUSP: 7577410
// NOPT: 0.080s
// OPTM: 0.064s

#include <stdio.h>

#define N 100000000

void computation1(int *);
void computation2(int *);
int computation3(int);
long sum;
int n[N];

int main() {
	#pragma omp parallel for schedule(static, 64) shared(n)
	for(int i = 0; i < N; i++)
		n[i] = i+1;

	#pragma omp parallel for schedule(static, 64) shared(n)
	for(int i = 0; i < N; i++)
		computation1(n+i);

	#pragma omp parallel for schedule(dynamic, 64) shared(n)
	for(int i = 0; i < N; i++)
		computation2(n+i);

	sum = 0;
	#pragma omp parallel for schedule(dynamic, 64) reduction(+: sum)
	for(int i = 0; i < N; i++)
		sum += computation3(n[i]);

	printf("sum = %ld\n", sum);
	//sum = 8079565777585793
	return 0;
}

void computation1(int *a) {
	*a = (1767321982 / (*a)) & 198203;
}

void computation2(int *a) {
	while(*a % 3 != 0) *a = (*a) / 3;
}

int computation3(int a) {
	if(a <= 3) return a;
	int i = 1, j = 2, k = 3, c;
	for(int q = 3; q < a; q++) {
		c = k*2 - j + i;
		i = j;
		j = k;
		k = c;
	}
	return k;
}
