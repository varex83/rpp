#include <iostream>
#include <random>
#include <omp.h>
#include <chrono>
#include <iomanip>

using namespace std;

const int N = 2e6 + 10;

bool sieve[N];

class Timer {
    private:
        chrono::high_resolution_clock::time_point start_time;
    public:
        Timer() {
            start_time = chrono::high_resolution_clock::now();
        }

        double elapsed() {
            auto end_time = chrono::high_resolution_clock::now();
            return chrono::duration_cast<chrono::duration<double> >(end_time - start_time).count();
        }
};

void sieve_naive(int n) {
    Timer timer;
    for (int i = 2; i <= n; i++) sieve[i] = true;
    for (int i = 2; i <= n; i++) {
        if (sieve[i]) {
            for (long long j = (long long)i * (long long)i; j <= n; j += i) sieve[j] = false;
        }
    }
    // Count total primes (using vector, because we use vector approach in other methods)
    vector<int> primes;
    for (int i = 2; i <= n; i++) {
        if (sieve[i]) primes.push_back(i);
    }
    cout << "[NAIVE] Total primes: " << primes.size() << endl;
    cout << "[NAIVE] Time taken: " << fixed << setprecision(6) << timer.elapsed() << " seconds" << endl;
}

int main() {

    int n = 1e6;

    sieve_naive(n);

    Timer timer;

    #pragma omp parallel for
    for (int i = 2; i <= n; i++) sieve[i] = true;

    #pragma omp parallel for
    for (int i = 2; i <= n; i++) {
        if (sieve[i]) {
            #pragma omp parallel for
            for (long long j = (long long)i * (long long)i; j <= n; j += i) sieve[j] = false;
        }
    }

    int ans = 0;
    #pragma omp parallel for reduction(+:ans)
    for (int i = 2; i <= n; i++) {
        if (sieve[i]) ans++;
    }

    cout << "[OMP] Total primes: " << ans << endl;
    cout << "[OMP] Time taken: " << fixed << setprecision(6) << timer.elapsed() << " seconds" << endl;

    return 0;
}