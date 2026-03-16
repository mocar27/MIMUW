#include <iostream>
#include <random>
#include <chrono>

// For n > 2000 would be very slow, as it's n^3 complexity.
// Naive matrix-matrix multiplication: C = alpha * A * B + beta * C
void my_dgemm(const double* A, const double* B, double* C,
              int n, double alpha = 1.0, double beta = 0.0) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = alpha * sum + beta * C[i * n + j];
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "invocation: " << argv[0] << " matrix_size " << std::endl;
        return 1;
    }
    const long long n{std::stoi(std::string{argv[1]})};

    std::mt19937_64 rnd;
    std::uniform_real_distribution<double> doubleDist{0, 1};

    double* A = new double[n * n];
    double* B = new double[n * n];
    double* C = new double[n * n];

    for (int i = 0; i < n * n; ++i) {
        A[i] = doubleDist(rnd);
        B[i] = doubleDist(rnd);
        C[i] = 0;
    }

    auto startTime = std::chrono::steady_clock::now();
    my_dgemm(A, B, C, n);
    auto finishTime = std::chrono::steady_clock::now();

    std::chrono::duration<double> elapsed{finishTime - startTime};
    std::cout << "Custom DGEMM elapsed time: " << elapsed.count() << " [s]" << std::endl;

    delete[] A;
    delete[] B;
    delete[] C;
    return 0;
}
