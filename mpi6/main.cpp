#include <cstdio>
#include <cstdlib>
#include <mpi.h>
#include <random>

MPI_Comm mpi_local_comm;

int calcGroupColor(int rank) {
	if (rank < 2)
		return -1;
	for (int i = 2; i * i <= rank; i++)
		if (rank % i == 0)
			return 1;
	return 0;
}

void doNothing(int globalRank) {
	printf("Hi there, I'm pretty useless process (global rank %i), and I am counting thumbs right now.\n", globalRank);
}

void customFunction(void* invec, void* inoutvec, int* len, MPI_Datatype* type) {
	if (*type == MPI_INT) {
        int* in = (int*)invec;
		int* out = (int*)inoutvec;
		for (int i = 0; i < *len; i++)
			out[i] += in[i];
    }
}

void doScan(int localRank, int size) {
	MPI_Op customOperation;
	MPI_Op_create(customFunction, true, &customOperation);
	int sentNum = localRank;
	printf("Hi, I'm proc #%i of %i from prime indices group, and I've got the following number: %i.\n", localRank, size, localRank);
	int recNum;
	MPI_Scan(&sentNum, &recNum, 1, MPI_INT, customOperation, mpi_local_comm);
	printf("Hi, I'm proc #%i of %i from prime indices group, and my result is %i.\n", localRank, size, recNum);
}

void doReduce(int localRank, int root, int size) {
	struct {
		int val;
		int rank;
	} sentPair, receivedPair;

	sentPair.rank = localRank;
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> valDistribution(0, 10000);
	sentPair.val = valDistribution(gen);

	MPI_Reduce(&sentPair, &receivedPair, 1, MPI_2INT, MPI_MINLOC, root, mpi_local_comm);
	if (localRank == root) {
		printf("Hi there, here's result of MINLOC reduce: min value is %i (from process #%i).\n",
			receivedPair.val, receivedPair.rank);
		printf("BTW, size of reduce group is %i\n", size);
	}
		
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int globalSize = 0, globalRank = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &globalSize);
	MPI_Comm_rank(MPI_COMM_WORLD, &globalRank);

	int groupColor = calcGroupColor(globalRank);
	MPI_Comm_split(MPI_COMM_WORLD, groupColor, globalRank, &mpi_local_comm);
	int localSize = 0, localRank = -1;
	MPI_Comm_size(mpi_local_comm, &localSize);
	MPI_Comm_rank(mpi_local_comm, &localRank);

	switch (groupColor) {
	case -1:
		doNothing(globalRank);
		break;
	case 0:
		doScan(localRank, localSize);
		break;
	case 1:
		doReduce(localRank, 0, localSize);
		break;
	}

	MPI_Comm_free(&mpi_local_comm);
	MPI_Finalize();

	return 0;
}
