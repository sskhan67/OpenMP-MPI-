#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// Mag and phase calculation -> Input: Horizontal and Vertical Gradient, Output: Magnitude and Phase

void gradient(float *v_grad, float *h_grad, float **magnitude , float **phase, int height, int width)
{
	*magnitude = (float*)malloc(sizeof(float)*height*width);
	*phase = (float*)malloc(sizeof(float)*height*width);
	#pragma omp parallel for  
	
	for(int i=0; i<height; i++) // loop to access pixels 
	{
		for(int j=0; j<width; j++)
		{

			*(*magnitude+i*width+j)=sqrt(pow(*(v_grad+i*width+j),2) + pow(*(h_grad+i*width+j),2));//magnitude
			*(*phase+i*width+j)=atan2(*(v_grad+i*width+j),*(h_grad+i*width+j));//Phase

		}
	}
	
}

