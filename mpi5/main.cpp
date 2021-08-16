#include <cstdio>
#include <mpi.h>
#include <vector>

int main(int argc, char** argv)
{
	MPI_Init(&argc, &argv);

	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Status status;

	//0---------------------------------------------------------------------------------------------
	long long unsigned rightVal = ((long long)rank + 1) * 2;
	long long unsigned leftVal = rightVal - 1;
	std::vector<long long unsigned> prods = std::vector<long long unsigned>();
	//1---------------------------------------------------------------------------------------------
	prods.push_back(rightVal * leftVal); //Mult #1
	
	if (rank == 0) {
		printf("1! = %i\n", 1);
		printf("2! = %llu\n", prods[0]);
	}
	//2---------------------------------------------------------------------------------------------
	if (rank % 2 == 0) //0,2,...
	{
		rightVal = (long long unsigned)rank * 2 + 2 + 1;
		leftVal = prods[0];
		MPI_Send(&prods[0], 1, MPI_LONG_LONG, rank + 1, rank, MPI_COMM_WORLD);
	}
	else //1,3,...
	{
		MPI_Recv(&leftVal, 1, MPI_LONG_LONG, rank - 1, rank - 1, MPI_COMM_WORLD, &status);
		rightVal = prods[0];
	}

	prods.push_back(rightVal * leftVal); //Mult #2
	

	if (rank < 2)
		printf("%i! = %llu\n", rank + 3, prods[1]);

	//3---------------------------------------------------------------------------------------------
	int prevRank = rank - 1;

	if ((rank + 2) % 4 == 0) //2, 6
		MPI_Send(&leftVal, 1, MPI_LONG_LONG, prevRank, rank, MPI_COMM_WORLD);

	if (prevRank % 4 == 0) //1, 5
	{
		MPI_Recv(&rightVal, 1, MPI_LONG_LONG, rank + 1, rank + 1, MPI_COMM_WORLD, &status);
		leftVal = prods[1];

		for (int i = (rank - 1); i < prevRank + 4; i++)
			if (i != rank)
				MPI_Send(&prods[1], 1, MPI_LONG_LONG, i, rank, MPI_COMM_WORLD);
	}
	else //0, 2, 3, 4, 6, 7
	{
		int sender = 4 * (rank / 4) + 1;
		MPI_Recv(&leftVal, 1, MPI_LONG_LONG, sender, sender, MPI_COMM_WORLD, &status);
		rightVal = prods[1];
	}

	if (rank % 4 == 0) //0, 4
		rightVal = (long long unsigned)rank * 2 + 4 + 1;

	prods.push_back(rightVal * leftVal); //Mult #3
	
	if (rank < 4)
		printf("%i! = %llu\n", rank + 5, prods[2]);

	//4---------------------------------------------------------------------------------------------
	if (rank == 5)
		MPI_Send(&prods[1], 1, MPI_LONG_LONG, 3, rank, MPI_COMM_WORLD);

	if (rank == 3)
	{
		MPI_Recv(&rightVal, 1, MPI_LONG_LONG, 5, 5, MPI_COMM_WORLD, &status);
		leftVal = prods[2];

		for (int i = 0; i < 8; i++)
			if (i != rank)
				MPI_Send(&prods[2], 1, MPI_LONG_LONG, i, rank, MPI_COMM_WORLD);
	}
	else
	{
		int sender = 8 * (rank / 8) + 3;
		MPI_Recv(&leftVal, 1, MPI_LONG_LONG, sender, sender, MPI_COMM_WORLD, &status);
	}

	if (rank > 3)
		rightVal = prods[2];
		

	if (rank == 4)
	{
		MPI_Send(&prods[0], 1, MPI_LONG_LONG, 1, rank, MPI_COMM_WORLD);
		MPI_Send(&prods[1], 1, MPI_LONG_LONG, 2, rank, MPI_COMM_WORLD);
	}
	
	if (rank == 1 || rank == 2)
		MPI_Recv(&rightVal, 1, MPI_LONG_LONG, 4, 4, MPI_COMM_WORLD, &status);

	if (rank % 8 == 0)
		rightVal = (long long unsigned)rank * 2 + 8 + 1;

	prods.push_back(rightVal * leftVal); //Mult #4
	
	if (rank < 8)
		printf("%i! = %llu\n", rank + 9, prods[3]);
	//---------------------------------------------------------------------------------------------

	MPI_Finalize();
	return 0;
}