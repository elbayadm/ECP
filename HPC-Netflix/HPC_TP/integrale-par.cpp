#include <cstdio>
#include <mpi.h>
#include <time.h>
clock_t start_t,end_t;
double elapsed;

float f (float x) {
	float return_val ; 
	return_val=4/(1+x*x);
	return return_val ;
}

float Trap (float local_a, float local_b, int local_n, float h){
	float integral; /* resultat */
	float x;
	integral = (f(local_a) + f(local_b))/2.0;
	x = local_a;
	for (int i = 1; i <= local_n-1; i++) {
		x += h;
		integral += f(x); }
	integral *= h;
	return integral;
}

int main(int argc, char* argv[]){
	start_t=clock();
	int my_rank;
	int p; /*Nb de processus*/
	float a = 0.0;
	float b = 1.0;
	int n = 1024;
	float h;
	float local_a;
	float local_b;
	int local_n;
	float integral; 
	float total; 
	int source; /*Emetteur*/
	int dest = 0; /*Tous les messages au 0*/
	int tag = 50;
	MPI_Status status;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	/*Communs*/
	h = (b-a)/n;
	local_n = n/p;
	/*Par processus*/
	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	integral = Trap(local_a, local_b, local_n, h);
	/*Le 0 collecte les résultats*/
	if (my_rank == 0) {
		total = integral;
		for (source = 1; source < p; source++) {
			MPI_Recv(&integral, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
			total += integral; }
		}
	else MPI_Send(&integral, 1, MPI_FLOAT, dest,tag, MPI_COMM_WORLD);
	end_t=clock();
	elapsed=(double)(end_t-start_t);
	if (my_rank == 0) {printf("Estimation : %f\n", total); printf("Elapsed time %.4f seconds \n",elapsed/CLOCKS_PER_SEC);}
	MPI_Finalize();
	return 0;
}
