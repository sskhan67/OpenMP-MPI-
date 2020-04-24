#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

int inBounds(int x, int y, int h, int w, int tgr, int bgr){
	if(x < 0 || x > w)
		return 0;
	else if(y < 0-tgr || y > h+bgr)
		return 0;
	else
		return 1;
}

int isConnected(float *hyst, int x, int y, int height, int width, int comm_rank, int comm_size){
	int tgr, bgr;
	
	if(comm_rank==0){
		tgr=0;
		bgr=1;
	}
	else if(comm_rank==(comm_size-1)){
		tgr=1;
		bgr=0;
	}
	else{
		tgr=1;
		bgr=1;
	}
	
	for(int y_offset=(-1); y_offset<=1; y_offset++){
		for(int x_offset=(-1); x_offset<=1; x_offset++){
			if(inBounds(x+x_offset, y+y_offset, height, width, 0, 0) &&
				*(hyst+(y+y_offset)*width+x_offset+x)==255){

				return 1;
			}
		}
	}
	return 0;
}




void find_edges(float *hyst, float **edges, int height, int width, int comm_rank, int comm_size){
	*edges = (float*)malloc(sizeof(float)*height*width);
	memcpy(*edges, hyst, sizeof(float)*height*width);
	#pragma omp parallel for  		
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			if(isConnected(hyst,j,i,height, width, comm_rank, comm_size))
				*(*edges+i*width+j)=255;
			else
				*(*edges+i*width+j)=0;
		}
	}
}
