#include <iostream>
#include <omp.h>

void calculo_a(double*, int) {}
void calculo_b(double*, int) {}
void calculo_c(double*, int) {}

double OpVec(double A[], double B[], double C[], int n);

int main() {
	int n = 10;
	double a[n], b[n], c[n];

	OpVec(a, b, c, n);

	return 0;
}

double OpVec(double A[], double B[], double C[], int n) {
	int i, j;
	double s1, s2, a, res;

	calculo_a(A, n); /* funcion calculo a */
	calculo_b(B, n); /* funcion calculo b */
	calculo_c(C, n); /* funcion calculo c */

	for (i = 0; i < n; i++) { /* primer bucle for*/
		s1 = 0;
		for (j = 0; j < n; j++) s1 += A[i] * B[i];
		for (j = 0; j < n; j++) A[i] *= s1;
	}

	for (i = 0; i < n; i++) { /* segundo bucle for */
		s2 = 0;
		for (j = 0; j < n; j++) s2 += B[i] * C[i];
		for (j = 0; j < n; j++) C[i] *= s2;
	}

	/* calculo final */
	a = s1 / s2;
	res = 0;

	for (i = 0; i < n; i++) res += a * C[i];

	return res;
}
