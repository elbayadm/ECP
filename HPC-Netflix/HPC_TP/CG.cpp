//Conjugate Gradient
// Maha -2015
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <climits>
#include <mpi.h>
#include <cmath>

using namespace std;
const int MASTER=0;
int my_rank; //Rank processor
int p; // processors number
MPI_Datatype STRIP;

//Prototypes
struct Matvect;
double ps(double* a, double* b, int n);
Matvect parse_data(string filename);
void pretty_print(double* a, int size, int id);
struct Matvect {
	double** Matrix=NULL;
	double* Matrix_data=NULL;
	double** Mat_local=NULL;
	double* Mat_local_data=NULL;
	double* product_vec=NULL;
	int nr,nc;
	double* vec;
	void product(){
		int part;
		if (p<=nr) part=nr/p;
	    if (p>nr) part=1;
	    int* counts=new int[p];
		int* displs=new int[p];

		if(p<=nr)
		{
			for (int i=0;i<p;i++){
				counts[i]=part;
				displs[i]=i*part;
		}
		counts[p-1]=nr-(p-1)*part;
		}
		else
			{
				for (int i=0;i<nr;i++){
				counts[i]=part;
				displs[i]=i*part;
				}
				for (int i=nr;i<p;i++){
				counts[i]=1;
				displs[i]=nr-1;
				}
			}
		//Defining row MPI_datatype:

		MPI_Type_vector(1,nc,nc, MPI_DOUBLE, &STRIP);
	    MPI_Type_commit(&STRIP);  
	    
		Mat_local=new double*[counts[my_rank]];
		Mat_local_data=new double[counts[my_rank]*nc];
	  	for (int k=0;k<counts[my_rank];k++) Mat_local[k]=&(Mat_local_data[k*nc]);
	 	 

		MPI_Scatterv(Matrix_data, counts,displs, STRIP, &(Mat_local[0][0]), nr, STRIP, MASTER, MPI_COMM_WORLD);
		//Compute partial  product: 
		double* partial_prod;
		partial_prod=new double[counts[my_rank]];

		for (int k=0; k<counts[my_rank];k++)
			partial_prod[k]=ps(Mat_local[k],vec,nr);
		
		product_vec=new double[nr];
		MPI_Allgatherv(partial_prod,counts[my_rank],MPI_DOUBLE,product_vec,counts,displs,MPI_DOUBLE,MPI_COMM_WORLD);
		//pretty_print(product_vec,nr,my_rank);

	}
};


double ps(double* a, double* b, int n){
    double product = 0;
    for (int i = 0; i < n; i++)
        product+=a[i]*b[i];
    return product;
	}

Matvect parse_data(string filename){
	Matvect M=Matvect();
	string line;
	int i=0;
	int j=0;
    double next;
    if(my_rank==MASTER)
    	{
    		ifstream infile (filename.c_str());
	    while (getline(infile, line)) {
	        if (i==0) {
	             istringstream iss(line);
	             iss>>M.nr>>M.nc;
	             M.Matrix_data = new double[(M.nr)*(M.nc)];
	             M.vec=new double[M.nc];
	             M.Matrix=new double*[M.nr];
				 for(int i = 0; i < M.nr; i++)
	              	M.Matrix[i] = &(M.Matrix_data[i*M.nc]);
	             i++;
	             continue;
	         }
	        

	        if (i<M.nr+1){
	            j=0;
	            istringstream iss(line);
	            while (iss >> next){
	                M.Matrix[i-1][j]=next;
	                j++;
	            }
	            i++;
	            continue;
	        }
	        
	        if(i>M.nr && i<M.nr+M.nc+1){
	            istringstream iss(line);
	            iss>>M.vec[i-M.nr-1];
	            i++;
	        }

		}
	}

	MPI_Bcast(&(M.nr),1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&(M.nc),1,MPI_INT,0,MPI_COMM_WORLD);
	if(my_rank!=MASTER) {
		M.vec=new double[M.nc];
	}
	MPI_Bcast(M.vec, M.nc, MPI_DOUBLE, 0, MPI_COMM_WORLD); 
	return M;
	}

void pretty_print(double* a, int size, int id){
	for(int i=0;i<size;i++)
		printf("From %d - %d: %f\n",id,i,a[i]);
}


int main(int argc, char* argv[]){

	string filename=argv[1];

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);
	printf("In here process %d\n",my_rank);
	Matvect M=parse_data(filename);
	MPI_Barrier(MPI_COMM_WORLD);
	M.product();
	if(my_rank==MASTER) 
	{
		for(int k=0;k<M.nr;k++)
			cout<<k+1<<": "<<M.product_vec[k]<<endl;
	}
		return 0;

	}

