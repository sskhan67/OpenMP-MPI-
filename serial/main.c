//author : Syeduzzaman khan
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image_template.h"
#include "convolution.h"
#include"gaussian_kernel.h"
#include "gradient.h"
#include "suppress.h"
#include "hysteresis.h"
#include "edge.h"
#include "cornerness.h"
#include "sys/time.h"


/*
Project Goal: To implement Canny Edge detection's 7 Feature detection and Parallelization using Open MP 

I/O File :  Test Image
Main function: Takes two argument as image path and sigma value=0.6 or 1 or 1.25

Setps: 

	User defined Header Files: image.h -> Image I/O handler, gaussian_kernel.h-> create gaussian kernel using user defined sigma value =0.6 or 1 or 1.25, convolution.h-> To perform
	convolution using Gaussian and Gaussian derivative kernel,  gradient.h-> perform horizontal and vertical gradient , boundary.h-> to handle boudary for convolution  
 hystersis+ suppress+edges 



*/


// main function starts here 
int main(int argc, char **argv)
{
		struct timeval start_com,start_end, end_com,end_end;
		// serial time measurement

		gettimeofday(&start_end,NULL);
		
		// declare variable 
		float *image,*g_kernel, *deriv_kernel, *temp_hor, *temp_ver,  *horizontal_grad, *vertical_grad, *magnitude, *phase,*sup,*hyst,*edge;	//  create pointer variable 
		int height, width, k_width;// image height, width, and hernel width size 
		float sigma=atof(argv[2]);

        	// step 1. call function to read image and pass the funtion to the function 
		read_image_template(argv[1],&image,&width,&height);

		// computation time measurement

		gettimeofday(&start_com,NULL);

		// step 2. Create gaussian kernel and gaussian derivative kernel using sigma 
		
		gaussian(&g_kernel, &deriv_kernel, atof(argv[2]), &k_width); // atof function converts sigma string value to type double 
	
	    

		// 3. Horizontal and Vertical Gradients are computed using 1-D Gaussian kernel and a Gaussian derivative kernel

		//3.1	Horizontal Gradient:

		convoultion(image, &temp_hor, g_kernel, height, width, k_width, 1); //smoothen first
		convoultion(temp_hor, &horizontal_grad, deriv_kernel, height, width, 1, k_width);//Horizontal Gradient:
		
		//3.2 Vertcal gradient
		convoultion(image, &temp_ver, g_kernel, height, width, 1, k_width);//smoothen first
		convoultion(temp_ver, &vertical_grad, deriv_kernel, height, width, k_width, 1);//Vertcal gradient
		
		// 4. Magnitude and Phase -> Input: Horizontal and Vertical Gradient, Output: Magnitude and Phase 
		gradient(vertical_grad, horizontal_grad, &magnitude, &phase, height, width);

		//5. Suppress 	
		suppress(magnitude, phase, &sup, height, width);

		//6. hysteresis

		hysteresis(sup, &hyst, height, width);
		//7 Edge linking 
		find_edges(hyst, &edge, height, width);
		//8. Feature detection
		feature_detector( height,width,vertical_grad,horizontal_grad);

		//print the compution time
		gettimeofday(&end_com,NULL);
		int time_comp=((end_com.tv_sec*1000000+end_com.tv_usec)-	(start_com.tv_sec*1000000+start_com.tv_usec))/1000;
		//printf("Image width: %d,Image Height: %d, Sigma: %f,Computation time (ms): %d\n",width,height,sigma,time_comp);


		// 9. Write image 
		write_image_template("Temp_Horizontal.pgm", temp_hor, width, height);// temp hori image
		write_image_template("Temp_Vertical.pgm", temp_ver, width, height);// temp ver image
		write_image_template("Horizontal_gradient.pgm", horizontal_grad, width, height); // hori gradient 
		write_image_template("Vertical_gradient.pgm", vertical_grad, width, height);// ver gradient image 
		write_image_template("Magitude.pgm", magnitude, width, height);// mag image 
		write_image_template("Phase.pgm", phase, width, height);// phase image 
		write_image_template("suppress.pgm", sup, width, height);
		write_image_template("hystersis.pgm",hyst, width, height);
		write_image_template("edges.pgm", edge, width, height);

//print the reuqired time
		gettimeofday(&end_end,NULL);
		int time_end=((end_end.tv_sec*1000000+end_end.tv_usec)-(start_end.tv_sec*1000000+start_end.tv_usec))/1000;
	printf("Image width: %d,Image Height: %d, Sigma: %f,Computation time (ms): %d\n, End to End time (ms): %d\n",width,height,sigma,time_comp,time_end);		


 
		free(image);
		free(g_kernel);
		free(deriv_kernel);
		free(temp_hor);
		free(temp_ver);
		free(horizontal_grad);
		free(vertical_grad);
		free(magnitude);
		free(phase);
		free(sup);
		free(hyst);
		free(edge);




}

