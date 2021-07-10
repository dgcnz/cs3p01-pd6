#include <iostream>
#include <mpi.h>

#define N 400

using namespace std;

int  f_beg(int rank);
void initialize_A_v(int A[][N], int v[]);
void print_matrix(int A[][N]);
void print_vector(int v[]);

int wsize;
int A[N][N], v[N], x[N];

int main(int argc, char **argv)
{
    int rank;

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &wsize);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0)
        initialize_A_v(A, v);

    int i_beg = f_beg(rank), i_end = f_beg(rank + 1);

    double start, finish;

    start = MPI_Wtime();
    // Distribute A and v from process 0 to all other processes
    if (rank == 0)
    {
        for (int i = 1; i < wsize; i++)
        {
            MPI_Send(A[f_beg(i)],
                     N * (f_beg(i + 1) - f_beg(i)),
                     MPI_INT,
                     i,
                     0,
                     MPI_COMM_WORLD);
            MPI_Send(v, N, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
    }
    else
    {
        MPI_Recv(A[i_beg],
                 N * (i_end - i_beg),
                 MPI_INT,
                 0,
                 0,
                 MPI_COMM_WORLD,
                 MPI_STATUS_IGNORE);
        MPI_Recv(v, N, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    // start = MPI_Wtime();
    // Multiply
#pragma omp parallel for default(none) shared(A, v, x) schedule(static, chunk)
    for (int i = i_beg; i < i_end; i++)
    {
        double xi = 0.0;
#pragma omp parallel for default(none) firstprivate(i) shared(A, v, xi) schedule(static,chunk) reduction(+:xi)
        for (int j = 0; j < N; j++)
            xi += A[i][j] * v[j];
        x[i] = xi;
    }
    // finish = MPI_Wtime();

    // Send x back from all processes to process 0
    if (rank != 0)
        MPI_Send(x + i_beg, i_end - i_beg, MPI_INT, 0, 0, MPI_COMM_WORLD);
    else
    {
        for (int i = 1; i < wsize; i++)
        {
            MPI_Recv(x + f_beg(i),
                     f_beg(i + 1) - f_beg(i),
                     MPI_INT,
                     i,
                     0,
                     MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }
    }
    finish = MPI_Wtime();

    MPI_Finalize();

    // Print result on the main process
    if (rank == 0)
    {
        // cout << "Result vector x:" << endl;
        // print_vector(x);

        cout << "TIME " << 1000 * (finish - start) << endl;
    }

    return 0;
}

int f_beg(int rank) { return N * rank / wsize; }

void initialize_A_v(int A[][N], int v[])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            A[i][j] = i;
        v[i] = i;
    }
}

void print_matrix(int A[][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            cout << A[i][j] << ' ';
        cout << endl;
    }
}

void print_vector(int v[])
{
    for (int j = 0; j < N; j++)
        cout << v[j] << endl;
}
