#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv){
	MPI_Init(&argc, &argv);
    
    int size = 0;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rank = 0;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    printf("I am #%d of %d.\n", rank, size);
    if (rank == 2){
        int message = 44;
        MPI_Send(&message, 1, MPI_INT, 0, 31, MPI_COMM_WORLD);
        printf("I've sent a message, and it's %d.\n", message);
    }
    if (rank == 0){
        int recievedMessage = -1;
        MPI_Status status;
        MPI_Recv(&recievedMessage, 1, MPI_INT, 2, 31, MPI_COMM_WORLD, &status);
        printf("Hey, we got a message here for you from %d. Here's the content: %d.", status.MPI_SOURCE, recievedMessage);
    }

    MPI_Finalize();
	return 0;
} 
