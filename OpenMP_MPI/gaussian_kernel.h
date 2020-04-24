#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// function 

void gaussian(float **g_kernel, float **deriv_kernel, float sigma, int *w)
{
	
	//1. Gaussain Kernel 
	int a = round(2.5*sigma-0.5); // round value to int 
	float sum = 0;
	
	*w=2*a+1; // kernel width 

	*g_kernel=(float*)malloc(sizeof(float)*(*w)); // allocation dynamic memory for kernel 

	for(int i=0; i<(*w); i++) // kernel main loop 
	{
		(*g_kernel)[i] = exp((-1*(i-a)*(i-a))/
			  (2*sigma*sigma));
		sum+=(*g_kernel)[i];			   
	}
	
	// Normalize
	#pragma omp parallel for  
	for(int i=0; i<(*w); i++)
	{
		(*g_kernel)[i]/=sum;	
	}


	// 2. Gaussian  Derivative

	sum = 0;
	*deriv_kernel=(float*)malloc(sizeof(float)*(*w));// dynamic memory allocation for deriv kernel 
	
	for(int i=0; i<(*w); i++) // main loop 
	{
		(*deriv_kernel)[i] = (-1*(i-a))*exp((-1*(i-a)*(i-a))/
			  (2*sigma*sigma));
		sum-=i*(*deriv_kernel)[i];			   
	}
	
	// Normalize
	for(int i=0; i<(*w); i++)
	{
		(*deriv_kernel)[i]/=sum;	
	}

	// flip derivative kernel   
    float *temp; // temp variable
	int start=0,end=*w-1;// start and end value of array 

    while(start<end) // main loop 
    {

     *temp=(*deriv_kernel)[start];
	 (*deriv_kernel)[start]=(*deriv_kernel)[end];
	 (*deriv_kernel)[end]=*temp;
	 start++;
	 end--;
    }
	free (temp);



}

