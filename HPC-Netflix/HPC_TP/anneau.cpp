#include <stdio.h>
#include <mpi.h>

int main(int argc, char *argv[]) {
    int numprocs, rank;
    int val=0;
    MPI_Status stat;


    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
   
    if (rank==0)
        MPI_Send(&val, 1, MPI_INT, 1, 1, MPI_COMM_WORLD);

    else if (rank!=numprocs-1){
        MPI_Recv(&val, 1, MPI_INT, rank-1, 1, MPI_COMM_WORLD, &stat);
        val+=rank;
        MPI_Send(&val, 1, MPI_INT, rank+1, 1, MPI_COMM_WORLD);
    }

    if (rank==numprocs-1){
         MPI_Recv(&val, 1, MPI_INT, rank-1, 1, MPI_COMM_WORLD, &stat);
         val+=rank;
         printf("Final output: %d.\n", val);
    }

    MPI_Finalize();
}
