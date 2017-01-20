#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int numprocs, rank, send_rank, recv_rank;
    int nData, nDataRecv, nTurns, nInitValue;
    MPI_Status stat;

    nTurns = -1;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // User input number of turns and initial data value
    do{
        if( rank == 0 ) {
            printf("Initial Value: \n");
            scanf("%d", &nInitValue);
            printf("Number of Rounds: \n");
            scanf("%d", &nTurns);
            MPI_Send( &nInitValue, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);
        }
//printf("In here: %d\n", rank);
        MPI_Bcast( &nInitValue, 1, MPI_INT, 0, MPI_COMM_WORLD );
        MPI_Bcast( &nTurns, 1, MPI_INT, 0, MPI_COMM_WORLD );
    } while(nTurns < 0);

    printf("Process %d out of %d got %d and %d\n", rank, numprocs, nTurns, nInitValue);

    if (rank == 0)
        recv_rank = numprocs - 1;
    else
        recv_rank = rank - 1;

    if (rank == numprocs - 1)
        send_rank = 0;
    else
        send_rank = rank + 1;

    if (rank == 0)
        MPI_Send(&nInitValue, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);

    do{
        MPI_Recv(&nDataRecv, 1, MPI_INT, recv_rank, 1, MPI_COMM_WORLD, &stat);
        nData = nDataRecv + rank;
        nTurns = nTurns - 1;
        if(nTurns !=0 || rank != 0) {
            printf("Process %d out of %d got %d and sent %d\n", rank, numprocs, nDataRecv, nData); // Print info
            MPI_Send(&nData, 1, MPI_INT, send_rank, 1, MPI_COMM_WORLD);
        }
    } while(nTurns > 0);

    printf("%d is done.\n", rank);

    MPI_Finalize();
}
