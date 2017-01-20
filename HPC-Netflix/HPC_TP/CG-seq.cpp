//Solving the linear system A*x=b
// Conjugate gradient method

#include <cstdio>
#include <cstdlib>
#include <string>
#include <math.h>
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;

const double tol=1e-5;
const int maxiter=1000;

double ps(double* a, double* b, int n);
double* vectorComb(double* dest, double* a, double* b,double s, double t, int size);
double* matvect( double** matrix, double* vec, int nr, int nc);
double nnorm(double* vec,int size);
void pretty_print(double* a, int size);
void copyVect(double*a, double* b, int size);

int main(int argc, char* argv[]){
	double** A;
	double* b;
	double* x;  //Aproached solution
	double* r; // Residual vector
	double* d; //distance
	double* prev_r; // Residual vector
	double alpha,beta;
	int nr=0;
	int i=0;
	int j=0;
	string line;
	double next;

	//REading the matrix and the vector:
	string filename=argv[1];
	ifstream infile (filename.c_str());
    while (getline(infile, line)) {
        if (i==0) {
             istringstream iss(line);
             iss>>nr;  //Symmetric matrix
             A=new double*[nr];
        	for(int k=0;k<nr;k++)
        		A[k]=new double[nr];
       		b=new double[nr];
       		x=new double[nr];
       		r=new double[nr];
       		d=new double[nr];
       		prev_r=new double[nr];
             i++; continue;
         }

        if (i<nr+1){
            j=0;
            istringstream iss(line);
            while (iss >> next){
                A[i-1][j]=next;
                j++;
            }
            i++;
            continue;
        }
        
        if(i>nr && i<2*nr+1){
            istringstream iss(line);
            iss>>b[i-nr-1];
            i++;
        }
    }
    //The Conjugate Gradient Loop
    //initial guess
    for(i=0;i<nr;i++){
    	x[i]=rand() % 10;
    }

  	vectorComb(r, b, matvect(A,x,nr,nr),1, -1, nr);
    copyVect(d,r,nr);
    int iter=0;
    while(nnorm(r,nr)>tol && iter<maxiter){
    	alpha=ps(r,r,nr)/ps(d,matvect(A,d,nr,nr),nr);
    	vectorComb(x,x,d,1,alpha,nr);
    	copyVect(prev_r,r,nr);
    	vectorComb(r,r,matvect(A,d,nr,nr),1,(-1)*alpha,nr);
    	beta=ps(r,r,nr)/ps(prev_r,prev_r,nr);
    	vectorComb(d,r,d,1,beta,nr);
    	printf("Iteration %d - Residual distance %f\n",iter,nnorm(r,nr));
    	iter++;
    }
    printf("The system solution:\n");
    pretty_print(x,nr);
    return 0;


}

double ps(double* a, double* b, int n){
    float product = 0;
    for (int i = 0; i < n; i++)
        product+=a[i]*b[i];
    return product;
	}


double* vectorComb(double* dest, double* a, double* b,double s, double t, int size) {
	int i;
	for (i = 0; i < size; i++)
		dest[i] = s*a[i] + t*b[i];
	return dest;
}

double* matvect( double** matrix, double* vec, int nr, int nc){
	double* dest=new double[nr];
	for (int k=0; k<nr;k++)
        dest[k]=ps(matrix[k],vec,nc);
    return dest;
}

double nnorm(double* vec,int size){
	return sqrt(ps(vec,vec,size));
}

void pretty_print(double* a, int size){
	for(int i=0;i<size;i++){
		printf("[%d]: %f\t",i,a[i]);
	printf("\n");
	}
}

void copyVect(double*a, double* b, int size){
	for(int i=0;i<size;i++)
		a[i]=b[i];
}