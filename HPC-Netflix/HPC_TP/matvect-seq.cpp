#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

const int Max_n=10;
using namespace std;
float ps(float a[], float b[], int n){
    float product = 0;
    for (int i = 0; i < n; i++)
        product+=a[i]*b[i];
    return product;
}

int main(int argc, char* argv[]){
    string filename=argv[1];
    string line;
    float Matrix[Max_n][Max_n];
    float vec[Max_n];
    float product_vec[Max_n];
    
      ifstream infile (filename.c_str());
    int nc=0;
    int nr=0;
    int i=0;
    int j=0;
    double next;
    
    //REading the matrix and the vector:
    while (getline(infile, line)) {
        if (i==0) {
             istringstream iss(line);
             iss>>nr>>nc;
             i++; continue;}
        
        
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
    // Master have built Matrix of size (nrxnc) and vector of size (ncx1)
    cout<<"product: "<<endl;
    for (int k=0; k<nr;k++){
        product_vec[k]=ps(Matrix[k],vec,nr);
        cout<<k+1<<" :"<<product_vec[k]<<endl;
    }
    return 0;
    
}