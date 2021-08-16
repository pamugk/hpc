#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define COUNT_OF_REPEATS 100000

//Проверка скорости работы MPI_Send, MPI_SSend и MPI_RSend
void benchmarkSafeSend(int destination, double tick, 
		int calledFunc, const char* functionName) {
	double message = 4815162342;
	int (*func)(const void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm) = calledFunc == 0 ? MPI_Send : calledFunc == 1 ? MPI_Ssend : MPI_Rsend;
	double start = MPI_Wtime();
	for (int i = 1; i <= COUNT_OF_REPEATS; i++)
		func(&message, 1, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD);
	double finish = MPI_Wtime();
	printf("%s method was executing %e microseconds in average (calculated with infelicity %e for %i repeats)\n", 
		functionName, (finish - start) / COUNT_OF_REPEATS * 1000000, tick, COUNT_OF_REPEATS);
}

//Проверка скорости работы MPI_Bsend
void benchmarkBufferedSend(int destination, double tick) 
{
	int buf_size = sizeof(double) * COUNT_OF_REPEATS + MPI_BSEND_OVERHEAD;
	char* buffer = malloc(buf_size);
	double message = 4815162342;
	double start = MPI_Wtime();
	MPI_Buffer_attach(buffer, buf_size);
	for (int i = 1; i <= COUNT_OF_REPEATS; i++)
		MPI_Bsend(&message, 1, MPI_DOUBLE, destination, 0, MPI_COMM_WORLD);
	MPI_Buffer_detach(buffer, &buf_size);
	double finish = MPI_Wtime();
	free(buffer);
	printf("MPI_Bsend method was executing %e microseconds in average (calculated with infelicity %e for %i repeats)\n",
		(finish - start) / COUNT_OF_REPEATS * 1000000, tick, COUNT_OF_REPEATS);
}

void benchmarkUnsafeSend(int destination, double tick) 
{
	double messages[COUNT_OF_REPEATS];
	for (int i = 0; i < COUNT_OF_REPEATS; i++)
		messages[i] = 4815162342;
	double start = MPI_Wtime();
	MPI_Request requests[COUNT_OF_REPEATS];
	MPI_Status statuses[COUNT_OF_REPEATS];
	for (int i = 0; i < COUNT_OF_REPEATS; i++)
		MPI_Isend(messages + i, 1, MPI_DOUBLE, destination, i, MPI_COMM_WORLD, requests + i);
	MPI_Waitall(COUNT_OF_REPEATS, requests, statuses);
	double finish = MPI_Wtime();
	printf("ISend method was executing %e microseconds in average (calculated with infelicity %e for %i repeats)\n",
		(finish - start) / COUNT_OF_REPEATS * 1000000, tick, COUNT_OF_REPEATS);
}

//Приём данных из MPI_Send, MPI_SSend и MPI_RSend
void recieveSafelySentMessage(int source, const char* functionName) 
{
	double receivedMessage;
	MPI_Status status;
	for (int i = 1; i <= COUNT_OF_REPEATS; i++)
		MPI_Recv(&receivedMessage, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, &status);
	printf("Data recieve from %s method completed.\n", functionName);
}

void recieveUnsafeMessage(int source) 
{
	double messages[COUNT_OF_REPEATS];
	MPI_Request requests[COUNT_OF_REPEATS];
	MPI_Status statuses[COUNT_OF_REPEATS];
	for (int i = 0; i < COUNT_OF_REPEATS; i++)
		MPI_Irecv(messages + i, 1, MPI_DOUBLE, source, i, MPI_COMM_WORLD, requests + i);
	MPI_Waitall(COUNT_OF_REPEATS, requests, statuses);
}

int main(int argc, char** argv) 
{
	MPI_Init(&argc, &argv);
	int rank = -1;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	switch (rank)
	{
	case 0: 
	{
		double tick = MPI_Wtick();
		benchmarkSafeSend(1, tick, 0, "MPI_Send");
		benchmarkSafeSend(1, tick, 1, "MPI_Ssend");
		benchmarkSafeSend(1, tick, 2, "MPI_Rsend");
		benchmarkBufferedSend(1, tick);
		benchmarkUnsafeSend(1, tick);
		break;
	}
	case 1: 
	{
		recieveSafelySentMessage(0, "MPI_Send");
		recieveSafelySentMessage(0, "MPI_Ssend");
		recieveSafelySentMessage(0, "MPI_Rsend");
		recieveSafelySentMessage(0, "MPI_Bsend");
		recieveUnsafeMessage(0);
		break;
	}
	default:
		break;
	}
	MPI_Finalize();
	return 0;
} 
