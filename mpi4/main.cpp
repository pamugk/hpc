#include <cmath>
#include <cstdio>
#include <mpi.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>

const int COUNT_OF_DOTS = 1000000;
const double a = 0;
const double b = M_PI / 2.;
const double yMin = 0;
const double yMax = 1;

double f(double x) { return cos(x); }

double fI() { return sin(b) - sin(a); }

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> xDistribution(a, b);
	std::uniform_real_distribution<> yDistribution(yMin, yMax);

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int countOfInnerPoints = 0;
	double start = MPI_Wtime();
	for (int i = 1; i <= COUNT_OF_DOTS / size; i++)
	{
		double x = xDistribution(gen);
		double y = yDistribution(gen);
		if (y <= f(x))
			countOfInnerPoints++;
	}

	printf("%i got %i inner points\n", rank, countOfInnerPoints);

	int totalCountOfInnerPoints;
	MPI_Reduce(&countOfInnerPoints, &totalCountOfInnerPoints, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

	if (rank == 0) {
		printf("Calculated integral: %f;\nPrecise integral: %f.\n", (b - a) * (yMax - yMin) * totalCountOfInnerPoints / COUNT_OF_DOTS, fI());
		printf("Time of calculation: %f seconds", (MPI_Wtime() - start) * 1000000);
	}

	MPI_Finalize();
	return 0;
}