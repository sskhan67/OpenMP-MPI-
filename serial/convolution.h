#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int inBounds1(int y, int x, int h, int w){
	if(x < 0 || x >= w){
		return 0;
	}
	else if(y < 0 || y >= h){
		return 0;
	}
	else
		return 1;
}

// convoultion function : core of the program, also main to source to parrallel programming, Inout: kernel, temp image, Output: Convoluated image 

void convoultion(float *image, float **output, float *g_kernel, int height, int width, int k_height, int k_width)

{
	
	*output = (float*)malloc(sizeof(float)*height*width); // allocate dynamic memory for convoluated image output 
	
	// 1. Iteration through pixels

	for(int i=0; i<height; i++)
	{
		for(int j=0; j<width; j++)
		{
			float sum = 0;

			// Iteration through  gaussian kernel
			for(int k=0; k<k_height;k++)
			{
				for(int m=0; m<k_width; m++)
				{
					int offseti = -1*floor(k_height/2)+k; // floor operation 
					int offsetj = -1*floor(k_width/2)+m;
					
					//if (i+offseti>0 && i+offseti<=height && j+offsetj>0 && j+offsetj<=width)
	if(inBounds1(j+offsetj, i+offseti, height, width))
					{
						sum+= (image[(i+offseti)*width+j+offsetj])*(g_kernel[k*k_width+m]);
						//sum+= *(image+(i+offseti)*width+j+offsetj)*(*(g_kernel+(k*k_width)+m));

					}
				}
			}
			*(*output+(i*width)+j)=sum;
		}
	}

//free(output);
}
