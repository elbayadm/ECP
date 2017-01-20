
#include <cstdio>
float f (float x) {
	float return_val ; 
	return_val=4/(1+x*x);
	return return_val ;
}

int main () {
/* resulat */
float a,b,h,x,integral ;
int n ;
printf ("Donner a, b, et n\n");
scanf ("%f %f %d", &a, &b, &n);
h = (b-a)/n ;
integral = (f(a) + f(b))/2 ;
x=a;
for(int i=1;i<=n-1;i++){
	x += h ;
	integral += f(x) ; 
}

integral *= h ;
printf("Estimation : %f\n", integral); 
return 0;
}