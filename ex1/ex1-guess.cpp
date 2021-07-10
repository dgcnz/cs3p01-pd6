#include <iostream>
#include <mpi.h>
#include <cstdlib>

#define N 1000    // Numero de intentos
#define A 6       // Numero de amigos
#define R 100     // Rango de numeros

using namespace std;

int guess() {
	return rand() % R + 1;
}

int size;

int main(int argc, char **argv) {
    int rank;

	srand(0);

    MPI_Init(NULL, NULL);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (rank == 0 && size < A) {
		cout << "ERROR: Need at least " << A << " processes to run this test." << endl;
		MPI_Abort(MPI_COMM_WORLD, 0);
	}
	
	int number, attempts = 0;
	bool global_guess = false;

	// El thread principal elige un numero secreto
	number = guess();

	if (rank == 0) {
		cout << "Thread 0 has chosen secret number " << number << endl;
	}

	MPI_Barrier(MPI_COMM_WORLD);

	while (attempts < N && !global_guess) {
		global_guess = true;

		if (rank != 0) {
			// Cada uno de los threads no principales adivina un numero
			number = guess();
			cout << "Thread 1 guesses " << number << endl;

			// Y se lo envia al thread principal para verificar si el grupo gana
			MPI_Send(&number, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
		}
		else {
			// El thread principal recibe los numeros de cada amigo
			for (int i = 1; i < A; i++) {
				int friends_number;
				MPI_Recv(&friends_number, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				
				// Y verifica que de forma global adivinen el numero
				global_guess &= friends_number == number;
			}
		}

		attempts++;

		// El thread principal le comunica a cada thread los resultados
		MPI_Bcast(&global_guess, 1, MPI_INT, 0, MPI_COMM_WORLD);

		if (rank == 0) {
			if (!global_guess) {
				cout << "Attempt " << attempts << " guess failed" << endl;
			}
		}

		MPI_Barrier(MPI_COMM_WORLD);
	}

    MPI_Finalize();

	// Print result on the main process
	if (rank == 0) {
		cout << (global_guess ? "Guessed!" : "Didn't guessed") << endl;
	}

	return 0;
}
