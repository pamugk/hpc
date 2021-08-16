#include <algorithm>
#include <cstdio>
#include <mpi.h>
#include <vector>

using namespace std;

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	const int N = 12;
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	if (N % size > 0)
	{
		printf("Oh, crap, your array can't be divided equally between threads");
		return -1;
	}

	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	vector<int> nums(N / size);
	printf("#%i (initial): ", rank);
	for (int i = 0; i < nums.size(); i++)
	{
		nums[i] = rand();
		printf("%i ", nums[i]);
	}
	sort(nums.begin(), nums.end());
	vector<int> received(N / size);
	MPI_Status status;
	if (rank % 2 == 0)
	{
		MPI_Send(&nums, nums.size(), MPI_INT, rank + 1, rank, MPI_COMM_WORLD);
		MPI_Recv(&received, received.size(), MPI_INT, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
	}
	else
	{
		MPI_Recv(&received, received.size(), MPI_INT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
		MPI_Send(&nums, nums.size(), MPI_INT, rank - 1, rank, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}
