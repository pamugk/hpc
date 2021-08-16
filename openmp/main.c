#define _USE_MATH_DEFINES
#include <math.h>
#include <omp.h>
#include <stdio.h>

#define h 0.01
#define N (int)(1 / h)

int main(int argc, char** argv) {
	const double tau = 0.1;
	const double mult = tau / (h * h);
	const int T = 1 / tau;
	const int countOfThreads = 4;

	double prevU[N * N];
	double u[N * N];
	omp_set_num_threads(countOfThreads);
    
    double *prevUHandler = prevU;
    double *uHandler = u;

	for (int i = 0; i < N * N; i++) {
		u[i] = 0, prevU[i] = 0;
	}

	double start = omp_get_wtime();
	for (double n = 1; n <= T; n++)
	{
		prevUHandler[0] = 1;
		#pragma omp parallel for
		for (int j = 0; j < N * N; j++)
		{
			int i = j / N, k = j % N;
			double temp = .0;
			if (i == 0)
				temp += prevUHandler[(i + 1) * N + k] - 2. * prevUHandler[i * N + k];
			else if (i == N - 1)
				temp += -2 * prevUHandler[i * N + k] + prevUHandler[(i - 1) * N + k];
			else
				temp += prevUHandler[(i + 1) * N + k] - 2 * prevUHandler[i * N + k] + prevUHandler[(i - 1) * N + k];

			if (k == 0)
				temp += prevUHandler[i * N + k + 1] - 2. * prevUHandler[i * N + k];
			else if (k == N - 1)
				temp += -2 * prevUHandler[i * N + k] + prevUHandler[i * N + k - 1];
			else
				temp += prevUHandler[i * N + k + 1] - 2 * prevUHandler[i * N + k] + prevUHandler[i * N + k - 1];

			u[i * N + k] = prevUHandler[i * N + k] + mult * temp;
		}
		double* swap = uHandler;
		uHandler = prevUHandler;
		prevUHandler = swap;
	}

	printf("Time of calculation: %f seconds\n", omp_get_wtime() - start);

	printf("Current state:\n");
	for (int i = 0; i < N; i += 1) {
		for (int k = 0; k < N; k += 1) {
			printf("%4.4g|", prevUHandler[i * (int)(N) + k]);
        }
		printf("\n");
	}
	
	return 0;
}
