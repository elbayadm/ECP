// Dot product
// Maha - 2015
#include <cstdio>
#include <mpi.h>
#include <iostream>
#include <cmath>
using namespace std;
const int MASTER=0;
const int MAX_buf=4000;
int main(int argc, char* argv[]){

    //Parallel part
	int my_rank;
	int p; //nb processors

	int vsize;
	float* a=NULL;
	float* b=NULL;
	
	
	float ps;	

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	


	if(my_rank==MASTER){
		cout << "Enter vectors size:"<<endl;
		cin >> vsize;
		a=new float[vsize];
		b=new float[vsize];
		cout << "Enter first vector"<<endl;
		for (int i=0;i<vsize;i++)
	 		cin>>a[i];
		cout << "Enter second vector"<<endl;
		for (int i=0;i<vsize;i++)
	 		cin>>b[i];
	}

	MPI_Bcast(&vsize,1,MPI_INT,0,MPI_COMM_WORLD);
	int* counts=new int[p];
	int* displs=new int[p];
	int part=vsize/p;

	for (int i=0;i<p;i++){
		counts[i]=part;
		displs[i]=i*part;
	}
	counts[p-1]=vsize-(p-1)*part;

	float ap[MAX_buf]={0};
	float bp[MAX_buf]={0};


	MPI_Scatterv(a,counts,displs,MPI_FLOAT,ap,vsize/p*2,MPI_FLOAT,MASTER,MPI_COMM_WORLD);
	MPI_Scatterv(b,counts,displs,MPI_FLOAT,bp,vsize/p*2,MPI_FLOAT,MASTER,MPI_COMM_WORLD);
	
	//Compute partial dot product: 
	int n=sizeof ap/sizeof (float);

	float ps_local=0;
	for (int i=0;i<n;i++) ps_local+=ap[i]*bp[i];

	MPI_Reduce(&ps_local, &ps, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);		

	if(my_rank==MASTER)
		cout<<"Dot product: "<<ps<<endl;
		
	
	MPI_Finalize();
	return 0;

	}