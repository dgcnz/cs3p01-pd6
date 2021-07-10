#include <iostream>
#include <omp.h>

void calculo_a(double *, int) {}
void calculo_b(double *, int) {}
void calculo_c(double *, int) {}

double OpVec(double A[], double B[], double C[], int n)
{
    double res;
#pragma omp parallel
#pragma omp single
    {
        double s1, s2;

#pragma omp task default(none) shared(A) firstprivate(n) depend(out : A)
        calculo_a(A, n); /* funcion calculo a */
#pragma omp task default(none) shared(B) firstprivate(n) depend(out : B)
        calculo_b(B, n); /* funcion calculo b */
#pragma omp task default(none) shared(C) firstprivate(n) depend(out : C)
        calculo_c(C, n); /* funcion calculo c */

#pragma omp task default(none) shared(A, B, s1) firstprivate(n) depend(in      \
                                                                       : A, B) \
    depend(out                                                                 \
           : A, s1)
        {
            for (int i = 0; i < n; i++)
            { /* primer bucle for*/
                s1 = 0;
                for (int j = 0; j < n; j++)
                    s1 += A[i] * B[i];
                for (int j = 0; j < n; j++)
                    A[i] *= s1;
            }
        }

#pragma omp task default(none) shared(B, C, s2) firstprivate(n) depend(in      \
                                                                       : B, C) \
    depend(out                                                                 \
           : C, s2)
        {
            for (int i = 0; i < n; i++)
            { /* segundo bucle for */
                s2 = 0;
                for (int j = 0; j < n; j++)
                    s2 += B[i] * C[i];
                for (int j = 0; j < n; j++)
                    C[i] *= s2;
            }
        }
#pragma omp task default(none) shared(res, C) firstprivate(s1, s2, n)          \
    depend(in                                                                  \
           : s1, s2, C) depend(out                                             \
                               : res)
        {
            /* calculo final */
            double a = s1 / s2;
            res      = 0;

            for (int i = 0; i < n; i++)
                res += a * C[i];
        }
#pragma omp taskwait
    }
    return res;
}

int main()
{
    int    n = 2;
    double a[n], b[n], c[n];
    a[0] = 1, a[1] = 2;
    b[0] = 1, b[1] = 2;
    c[0] = 1, c[1] = 2;

    double res = OpVec(a, b, c, n);

    printf("%f\n", res);

    return 0;
}
