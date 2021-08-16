#include <cmath>
#include <cstdio>
#include <omp.h>
#define _USE_MATH_DEFINES
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
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> xDistribution(a, b);
	std::uniform_real_distribution<> yDistribution(yMin, yMax);
	omp_set_num_threads(4);

	int countOfInnerPoints = 0;
	double start = omp_get_wtime();
	#pragma omp parallel for reduction(+: countOfInnerPoints)
	for (int i = 1; i <= COUNT_OF_DOTS; i++)
	{
		double x = xDistribution(gen);
		double y = yDistribution(gen);
		if (y <= f(x))
			countOfInnerPoints++;
	}
	printf("Calculated integral: %f;\nPrecise integral: %f.\n", (b - a) * (yMax - yMin) * countOfInnerPoints / COUNT_OF_DOTS, fI());
	printf("Time of calculation: %f seconds\n", omp_get_wtime() - start);

	countOfInnerPoints = 0;
	start = omp_get_wtime();
	#pragma omp parallel for
	for (int i = 1; i <= COUNT_OF_DOTS; i++)
	{
		double x = xDistribution(gen);
		double y = yDistribution(gen);
		if (y <= f(x))
			#pragma omp atomic
			countOfInnerPoints++;
	}
	printf("Calculated integral: %f;\nPrecise integral: %f.\n", (b - a) * (yMax - yMin) * countOfInnerPoints / COUNT_OF_DOTS, fI());
	printf("Time of calculation: %f seconds\n", omp_get_wtime() - start);
	return 0;
}
