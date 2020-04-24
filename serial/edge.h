#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


int inBounds(int y, int x, int h, int w){
	if(x < 0 || x >= w){
		return 0;
	}
	else if(y < 0 || y >= h){
		return 0;
	}
	else
		return 1;
}

int isConnected(float *hyst, int x, int y, int height, int width){
	for(int y_offset=-1; y_offset<=1; y_offset++){
		for(int x_offset=-1; x_offset<=1; x_offset++){
			if(inBounds(x+x_offset, y+y_offset, height, width) &&
				*(hyst+(y+y_offset)*width+x_offset+x)==255)
				return 1;	
		}
	}
	return 0;
}



void find_edges(float *hyst, float **edges, int height, int width){
	*edges = (float*)malloc(sizeof(float)*height*width);
	memcpy(*edges, hyst, sizeof(float)*height*width);	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			if(isConnected(hyst,j,i,height, width))
				*(*edges+i*width+j)=255;
			else
				*(*edges+i*width+j)=0;
		}
	}
}
