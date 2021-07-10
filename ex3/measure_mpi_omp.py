import matplotlib.pyplot as plt
from subprocess import check_output

TESTS = 100
P_TESTS = [4, 8, 16]
T_TESTS = [4, 8]
COMMAND = "mpirun  --mca opal_warn_on_missing_libcuda 0 -np {0} ./mpi_omp.out {1}"


def measure_time(p, t):
    total = 0.0

    for i in range(TESTS):
        out = check_output(COMMAND.format(p, t).split()).decode("utf-8")
        total += float(out)

    total /= TESTS

    return total


def main():
    single_p_time = measure_time(1, 1)  # single processor

    plt.figure(figsize=(10, 8))
    plt.title("Speedup vs. number of processes")
    plt.ylabel("Speedup")
    plt.xlabel("Number of processes")
    for t in T_TESTS:
        tests = []
        for p in P_TESTS:
            print("Process {} {}".format(p, t))
            test = measure_time(p, t) / single_p_time  # speedup
            tests.append(test)
        plt.plot(P_TESTS, tests, label=f"{t} threads.")

    plt.savefig("mpi_omp.png")


if __name__ == "__main__":
    main()
