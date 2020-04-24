#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

float left(float *arr, int x, int y, int width){
	if(x-1>=0){
		return *(arr+y*width+(x-1));
	}
	return (float)NULL;
}

// returns null if OOB, else value
float right(float *arr, int x, int y, int width){
	if(x+1<width){
		return *(arr+y*width+(x+1));
	}
	return (float)NULL;
}

// returns null if OOB, else value
float top(float *arr, int x, int y, int height, int width){
	if(y-1>=0){
		return *(arr+(y-1)*width+x);
	}
	return (float)NULL;
}

// returns null if OOB, else value
float bottom(float *arr, int x, int y, int height, int width){
	if(y+1<height){
		return *(arr+(y+1)*width+x);
	}
	return (float)NULL;
}

// returns null if OOB, else value
float topLeft(float *arr, int x, int y, int height, int width){
	if(x-1>=0 && y-1>=0){
		return *(arr+(y-1)*width+(x-1));
	}
	return (float)NULL;
}

// returns null if OOB, else value
float bottomRight(float *arr, int x, int y, int height, int width){
	if(x+1<width && y+1<height){
		return *(arr+(y+1)*width+(x+1));
	}
	return (float)NULL;
}

// returns null if OOB, else value
float topRight(float *arr, int x, int y, int height, int width){
	if(x+1<width && y-1>=0){
		return *(arr+(y-1)*width+(x+1));
	}
	return (float)NULL;
}

// returns null if OOB, else value
float bottomLeft(float *arr, int x, int y, int height, int width){
	if(x-1>=0 && y+1<height){
		return *(arr+(y+1)*width+(x-1));
	}
	return (float)NULL;
}


void suppress(float *mag, float *phase, float **sup, int height, int width){
	*sup = (float*)malloc(sizeof(float)*height*width);
	memcpy(*sup, mag, sizeof(float)*height*width);	
	for(int i=0; i<height; i++){
		for(int j=0; j<width; j++){
			float theta = *(phase+i*width+j);
			if(theta<0)
				theta+=M_PI;
			
			theta*=(180/M_PI);			

			if(theta<=22.5 || theta >157.5){
				if(left(mag,j,i,width) > *(mag+i*width+j)
				   	|| right(mag,j,i,width) > *(mag+i*width+j)){
					*(*sup+i*width+j)=0; 
				}
			}
			else if(theta>22.5 && theta<=67.5){
				if(topLeft(mag,j,i,width,height) > *(mag+i*width+j)
					|| bottomRight(mag,j,i,height, width) > *(mag+i*width+j)){
					*(*sup+i*width+j)=0; 
				}
			}
			else if(theta>67.5 && theta<=112.5){
				if(top(mag,j,i,height,width) > *(mag+i*width+j) 
					|| bottom(mag,j,i,height,width) > *(mag+i*width+j)){
					*(*sup+i*width+j)=0; 
				}
			}
			else if(theta>112.5 && theta<=157.5){
				if(topRight(mag,j,i,height,width) > *(mag+i*width+j) 
					|| bottomLeft(mag,j,i,height,width) > *(mag+i*width+j)){
					*(*sup+i*width+j)=0; 
					}
			}
		}
	}
}
