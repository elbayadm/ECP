// Sequential Dot product
// Maha -2015
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <iostream>
using namespace std;

double ps(vector<double> a, vector<double> b){
	if (a.size()!=b.size()) {
		printf("Sizes don't match");
		exit (EXIT_FAILURE);
	}
	int n=a.size();
	double product = 0;
    for (int i = 0; i <= n-1; i++)
            product+=a[i]*b[i];
    return product;
}


int main(){
	double input;
	int vsize;
	vector<double> a,b;
	cout << "Enter the vectors size: " << endl;
	cin >> vsize;
	cout << "Enter the first vector: " << endl;
 	for (int i = 0; i < vsize; i++)
 	{
          cin >> input;
          a.push_back(input);
      }
	cout << "la taille du 1er vecteur: " << a.size() << endl;
	cout << "Enter the second vector: " << endl;
	for (int i = 0; i < vsize; i++)
	{
          cin >> input;
          b.push_back(input);
      }
    cout << "la taille du 2eme vecteur: " << b.size() << endl;

	double product=ps(a,b);
	cout << "Le produit scalaire: " << product << endl;
    return 0;
}