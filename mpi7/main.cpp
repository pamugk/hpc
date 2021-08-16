#include <cstdio>
#include <mpi.h>
#include <random>

void useStandardGather(void* sendbuf, int root)
{
	int size = 0, rank = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int* recBuf = new int[size];
	double start = MPI_Wtime();
	MPI_Gather(sendbuf, 1, MPI_INT, recBuf, 1, MPI_INT, root, MPI_COMM_WORLD);
	double end = MPI_Wtime();
	if (rank == root) {
		printf("It's root proc (#%i), and here are all of the received rand numbers:\n", rank);
		for (int i = 0; i < size; i++)
			printf("\t%i\n", recBuf[i]);
		printf("Total time of gathering all values: %f seconds\n", (end - start));
	}
	delete[] recBuf;
}

void myGather(
	void* sendbuf, int sendcount, MPI_Datatype sendtype,
	void* recvbuf, int recvcount, MPI_Datatype recvtype,
	int root, MPI_Comm comm)
{
	int size = 0, rank = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	if (rank != root)
	{
		MPI_Ssend(sendbuf, sendcount, sendtype, root, rank, comm);
		return;
	}

	for (int i = 0; i < size; i++)
	{
		if (i == rank)
			for (int j = 0; j < recvcount; j++)
				((int*)recvbuf)[i * recvcount + j] = ((int*)sendbuf)[j];
		else 
		{
			MPI_Status status;
			int recv;
			MPI_Recv(&recv, recvcount, recvtype, i, i, comm, &status);
			((int*)recvbuf)[i] = recv;
		}
	}
}

void useMyGather(void* sendbuf, int root)
{
	int size = 0, rank = -1;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int* recBuf = new int[size];
	double start = MPI_Wtime();
	myGather(sendbuf, 1, MPI_INT, recBuf, 1, MPI_INT, root, MPI_COMM_WORLD);
	double end = MPI_Wtime();
	if (rank == root) {
		printf("It's root proc (#%i), and here are all of the received rand numbers (custom):\n", rank);
		for (int i = 0; i < size; i++)
			printf("\t%i\n", recBuf[i]);
		printf("Total time of custom gathering all values: %f seconds\n", (end - start));
	}
	delete[] recBuf;
}

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> valDistribution(0, 10000);
	int sentVal = valDistribution(gen);
	useStandardGather(&sentVal, 0);
	useMyGather(&sentVal, 0);
	MPI_Finalize();
	return 0;
}