import matplotlib.pyplot as plt
from subprocess import check_output

TESTS = 100
P_TESTS = [2, 4]  # , 8, 16
COMMAND = "mpirun -np {0} ./ex3.out"


def measure_time(p):
    total = 0.0

    for i in range(TESTS):
        out = check_output(COMMAND.format(p).split()).decode("utf-8")
        total += float(out.split()[1])

    total /= TESTS

    return total


def main():
    single_p_time = measure_time(1)  # single processor

    tests = []

    for p in P_TESTS:
        print("Process {}".format(p))
        test = measure_time(p) / single_p_time  # speedup
        tests.append(test)

    plt.figure(figsize=(10, 8))
    plt.title("Speedup vs. number of processes")
    plt.ylabel("Speedup")
    plt.xlabel("Number of processes")
    plt.plot(P_TESTS, tests)
    plt.savefig("mpi.png")


if __name__ == "__main__":
    main()
