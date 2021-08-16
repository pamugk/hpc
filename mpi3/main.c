#include <stdio.h>
#include <mpi.h>

const int COUNT_OF_CYCLES = 2;

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int value;
	if (rank == 0)
	{
		value = 1;
		MPI_Status status;
		for (int i = 1; i <= COUNT_OF_CYCLES; i++)
		{
			printf("Here's thread #0. Right now I start cycle #%i of a relay race with value %i.\n", i, value);
			value *= 2;
			MPI_Send(&value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
			printf("I've just started cycle #%i of a relay race, because I've sent doubled value (%i) to the next thread.\n",
				i, value);
			MPI_Recv(&value, 1, MPI_INT, size-1, size - 1, MPI_COMM_WORLD, &status);
			printf("Thread #0 is still here. Just a moment ago cycle #%i of a relay race was finished with value %i.\n\n",
				i, value);
		}
		printf("Relay race was finally finished after %i cycles", COUNT_OF_CYCLES);
	}
	else
	{
		value = -1;
		MPI_Status status;
		int nextThread = rank == size - 1 ? 0 : rank + 1;
		for (int i = 1; i <= COUNT_OF_CYCLES; i++)
		{
			MPI_Recv(&value, 1, MPI_INT, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
			printf("Here's thread #%i. It's cycle #%i of a relay race, and I've just received value %i from thread #%i.\n", 
				rank, i, value, rank - 1);
			value *= 2;
			MPI_Send(&value, 1, MPI_INT, nextThread, rank, MPI_COMM_WORLD);
			printf("Thread #%i is still here. I've continued cycle #%i of a relay race and sent value %i to thread %i.\n\n",
				rank, i, value, nextThread);
		}
	}
	MPI_Finalize();
	return 0;
} 
