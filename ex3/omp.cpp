#include <assert.h>
#include <omp.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

const int N = 16;

int main(int argc, char *argv[])
{
    assert(argc == 2);

    vector<vector<int>> A(N, vector<int>(N, 2));
    vector<int> V(N, 2), X(N);

    const int P = stoi(argv[1]);
    const int B = N / P;
    double start = omp_get_wtime();

#pragma omp parallel num_threads(P)
    {
        int t = omp_get_thread_num();
        for (int i = t * B; i < min(t * B + B, N); ++i)
        {
            for (int j = 0; j < N; ++j)
            {
                X[i] += A[i][j] * V[j];
            }
        }
    }

    double finish = omp_get_wtime();

    printf("%f", 1000.0 * (finish - start));

    return 0;
}