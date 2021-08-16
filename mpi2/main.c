#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) 
{
	MPI_Init(&argc, &argv);
	int size = 0;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	bool deadlock = false;

	if (rank == 0)
	{
        printf("Deadlock app (0 - no, 1 - yes): ");
		char answer = getchar();
		if (answer == '1')
			deadlock = true;
		for (int i = 1; i < size; i++)
			MPI_Send(&deadlock, 1, MPI_C_BOOL, i, 0, MPI_COMM_WORLD);

		if (deadlock)
		{
			for (int i = 1; i < size; i++)
			{
				char receivedMessage[20];
				MPI_Status status;
				MPI_Recv(&receivedMessage, 20, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
				printf("We've got a message for you:'%s'\n", receivedMessage);
			}
			char sentMessage[] = "DEADLOCK";
			for (int i = 1; i < size; i++)
				MPI_Send(&sentMessage, 9, MPI_CHAR, i, 0, MPI_COMM_WORLD);
		}
		else
		{
			char sentMessage[] = "DEADLOCK RESOLVED";
			for (int i = 1; i < size; i++)
				MPI_Send(&sentMessage, 18, MPI_CHAR, i, 0, MPI_COMM_WORLD);
			for (int i = 1; i < size; i++)
			{
				char receivedMessage[31];
				MPI_Status status;
				MPI_Recv(&receivedMessage, 31, MPI_CHAR, i, i, MPI_COMM_WORLD, &status);
				printf("We've got a message for you:'%s'\n", receivedMessage);
			}
		}
	}
	else
	{
		MPI_Status stat;
		MPI_Recv(&deadlock, 1, MPI_C_BOOL, 0, 0, MPI_COMM_WORLD, &stat);

		if (deadlock)
		{
			char receivedMessage[9];
			MPI_Recv(&receivedMessage, 9, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
			printf("We've got a message for you:'%s'\n", receivedMessage);
			char sentMessage[] = "YES, IT IS DEADLOCK";
			MPI_Send(&sentMessage, 20, MPI_CHAR, 0, rank, MPI_COMM_WORLD);
		}
		else
		{
			char receivedMessage[18];
			MPI_Recv(&receivedMessage, 18, MPI_CHAR, 0, 0, MPI_COMM_WORLD, &stat);
			printf("We've got a message for you:'%s'\n", receivedMessage);
			char sentMessage[] = "CONFIRM, DEADLOCK WAS RESOLVED";
			MPI_Send(&sentMessage, 31, MPI_CHAR, 0, rank, MPI_COMM_WORLD);
		}
	}
	MPI_Finalize();
	return 0;
} 
