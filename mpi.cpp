#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>

using namespace std;

bool divisible(int n, const vector<int>& primes) {
    for (int prime : primes) {
        if (n % prime == 0) return true;
    }
    return false;
}

void master_process(int n, int rank, int size) {
    vector<int> primes;
    int sqn = (int)sqrt(n);
    vector<bool> sieve(sqn + 1, true);
    for (int i = 2; i <= sqn; i++) {
        if (sieve[i]) {
            primes.push_back(i);
            for (int j = i * i; j <= sqn; j += i) {
                sieve[j] = false;
            }
        }
    }

    int primes_size = primes.size();
    for (int i = 0; i < size - 1; i++) {
        MPI_Send(&primes_size, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        MPI_Send(primes.data(), primes_size, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
    }

    vector<int> total_primes;
    for (int i = 0; i < size - 1; i++) {
        int numbers_size;
        MPI_Recv(&numbers_size, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        vector<int> numbers(numbers_size);
        MPI_Recv(numbers.data(), numbers_size, MPI_INT, i + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        for (int number : numbers) {
            total_primes.push_back(number);
        }
    }

    cout << "[MPI] Total primes: " << total_primes.size() << endl;
}

void worker_process(int n, int rank, int size) {
    int primes_size;
    MPI_Recv(&primes_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    vector<int> primes(primes_size);
    MPI_Recv(primes.data(), primes_size, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    int l = (rank == 1) ? 2 : ((rank - 1) * (n / (size - 1)));
    int r = (rank == size - 1) ? n : (rank * (n / (size - 1)) - 1);
    
    vector<bool> sieve(r - l + 1, true);
    for (auto prime : primes) {
        int start = (l + prime - 1) / prime * prime;
        if (start < prime * prime) start = prime * prime;

        for (int i = start; i <= r; i += prime) {
            sieve[i - l] = false;
        }
    }


    vector<int> numbers;
    for (int i = 0; i < sieve.size(); i++) {
        if (sieve[i]) {
            numbers.push_back(i + l);
        }
    }

    int numbers_size = numbers.size();
    MPI_Send(&numbers_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Send(numbers.data(), numbers_size, MPI_INT, 0, 0, MPI_COMM_WORLD);
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int n = 1e6;

    int rank, size;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    double start_time = MPI_Wtime();

    if (rank == 0) {
        master_process(n, rank, size);
    } else {
        worker_process(n, rank, size);
    }

    double end_time = MPI_Wtime();
    double total_time = end_time - start_time;

    if (rank == 0)
    {
        std::cout << "[MPI] Time taken: " << total_time << " seconds" << std::endl;
    }

    MPI_Finalize();
    return 0;
} 