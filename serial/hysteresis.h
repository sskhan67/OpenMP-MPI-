#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

// Returns neg if b > a abd pos of a > b
int floatcomp(const void *a, const void *b){
	if(*(const float*)a < *(const float*)b)
		return -1;
	return *(const float*)a > *(const float*)b;
}


void hysteresis(float *sup, float **hyst, int height, int width){
	float *sorted = (float*)malloc(sizeof(float)*height*width);
	float t_high, t_low;
	
	*hyst = (float*)malloc(sizeof(float)*height*width);
	memcpy(*hyst, sup, sizeof(float)*height*width);	
	memcpy(sorted, sup, sizeof(float)*height*width);	
	
	qsort(sorted, height*width, sizeof(float), floatcomp);
	t_high = *(sorted+(int)(0.95*height*width));
	t_low = t_high/5;
	

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
	free(sorted);
}
