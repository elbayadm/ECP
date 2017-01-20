//Product of Matrix with vector
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
const int MAX_buf=4000;
const int Max_n=1000;



float ps(float* a, float* b, int n){
    float product = 0;
    for (int i = 0; i < n; i++)
        product+=a[i]*b[i];
    return product;
}

int main(int argc, char* argv[]){

	int my_rank;
	int p; //nb processors

	string filename=argv[1];
    string line;

    float** Matrix=NULL;
    float** Mat_local=NULL;
    float* Matrix_data=NULL;
    float* Mat_local_data=NULL;
    float* vec;
	int nr,nc;
    int part=0;
    int i=0;

    int j=0;
    double next;
    MPI_Datatype STRIP;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &p);

	
	//REading the matrix and the vector:

	if(my_rank==MASTER){ 
	ifstream infile (filename.c_str());   
    while (getline(infile, line)) {
        if (i==0) {
             istringstream iss(line);
             iss>>nr>>nc;
             Matrix_data = new float[nr*nc];
             vec=new float[nc];
             Matrix=new float*[nr];
			 for(int i = 0; i < nr; i++)
              	Matrix[i] = &(Matrix_data[i*nc]);
             i++;
             continue;
         }
        
        

        if (i<nr+1){
            j=0;
            istringstream iss(line);
            while (iss >> next){
                Matrix[i-1][j]=next;
                j++;
            }
            i++;
            continue;
        }
        
        if(i>nr && i<nr+nc+1){
            istringstream iss(line);
            iss>>vec[i-nr-1];
            i++;
        }
    }
    if (p<=nr) part=nr/p;
    if (p>nr) part=1;
    printf("All file read from %d (MASTER) -> nr: %d & nc: %d\n",my_rank,nr,nc);
	}
	// Master have built Matrix of size (nr x nc) and vector of size (nc x 1)}
	MPI_Bcast(&nr,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&nc,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&part,1,MPI_INT,0,MPI_COMM_WORLD);

	if(my_rank!=MASTER) vec=new float[nr];
	MPI_Bcast(vec, nc, MPI_FLOAT, 0, MPI_COMM_WORLD); 
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
	

	
	printf("In here process %d - share: %d\n",my_rank,counts[my_rank]);
	//Defining MPI datatypes:

	MPI_Type_vector(1,nc, nc, MPI_FLOAT, &STRIP);
    MPI_Type_commit(&STRIP);  
    
	Mat_local=new float*[counts[my_rank]];
	Mat_local_data=new float[counts[my_rank]*nc];
  	for (int k=0;k<counts[my_rank];k++) Mat_local[k]=&(Mat_local_data[k*nc]);
 	 

	MPI_Scatterv(Matrix_data, counts,displs, STRIP, &(Mat_local[0][0]), nr, STRIP, MASTER, MPI_COMM_WORLD);
	//Compute partial  product: 
	float* partial_prod;
	
	partial_prod=new float[counts[my_rank]];
	

	for (int k=0; k<counts[my_rank];k++)
		partial_prod[k]=ps(Mat_local[k],vec,nr);
	
	float* product_vec=new float[nr];

	MPI_Gatherv(partial_prod,counts[my_rank],MPI_FLOAT,product_vec,counts,displs,MPI_FLOAT,MASTER,MPI_COMM_WORLD);
	if (my_rank==MASTER){
		cout<<"---------------"<<endl<<" Product vector"<<endl<<"---------------"<<endl;
		for (int k=0; k<nr;k++)
			cout<<k+1<<" :"<<product_vec[k]<<endl;
	}

	MPI_Finalize();
	return 0;



	}