#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>




int floatcomp(const void *a, const void *b){
	if(*(const float*)a < *(const float*)b)
		return -1;
	return *(const float*)a > *(const float*)b;
}


void hysteresis(float *sup, float **hyst, int height, int width, float t_high, float t_low){

	
	*hyst = (float*)malloc(sizeof(float)*height*width);
	memcpy(*hyst, sup, sizeof(float)*height*width);	
	
	#pragma omp parallel for  
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			if(*(sup+i*width+j)>=t_high)
				*(*hyst+i*width+j)=255;
			else if(*(sup+i*width+j)<=t_low)
				*(*hyst+i*width+j)=0;
			else if(*(sup+i*width+j)<t_high && *(sup+i*width+j)>t_low)
				*(*hyst+i*width+j)=125;
		}
	}

}


