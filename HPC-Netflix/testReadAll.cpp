#include <cstdio>
#include <cmath>
#include <map>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>
#include <cstring>
#include <numeric>
#include <mpi.h>
#define MAX_MOVIES      17771

using namespace std;
void pretty_print(int rank,int* a, int size);
int main(int argc, char* argv[]){

    MPI_Init(&argc, &argv);

    int p,rank;
    MPI_File ratingsFile;
    MPI_Status *st;

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_File_open(MPI_COMM_WORLD,"movies.bin",MPI_MODE_RDONLY,MPI_INFO_NULL,&ratingsFile);
    if (ratingsFile==NULL) {
        printf("File error, movies.bin\n");
        exit(1);
        }
    MPI_Barrier(MPI_COMM_WORLD);
    printf("[%d]\t \"movies.bin\" open \n",rank);
    int holdit[10];
    MPI_File_read_at_all(ratingsFile,10*rank*sizeof(int),holdit, 10, MPI_INT, st);
    MPI_File_close(&ratingsFile);
    pretty_print(rank,holdit,10);
    MPI_Finalize();
    return 0;
}

void pretty_print(int rank,int* a, int size){
        for(int i=0;i<size;i++){
            printf("[%d]\t[%d]:%d\n",rank,i,a[i]);
        }
    }