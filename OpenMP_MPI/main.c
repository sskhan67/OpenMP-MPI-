//author : Syeduzzaman khan
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "image_template.h"
#include "sys/time.h"
#include "mpi.h"
#include "omp.h"
#include "sys/time.h"
#include "convolution.h"
#include"gaussian_kernel.h"
#include "gradient.h"
#include "suppress.h"
#include "hysteresis.h"
#include "edge.h"
#include "cornerness.h"
#include "ghost_row.h"


// Goal : Canny edge detector speedup using OpenMP & MPI 


// main function starts here 
int main(int argc, char **argv)
{
		// declare variable for time computation 		
		struct timeval start_comp,start, end_comp,end;

		
		// declare variable 
		float *image,*g_kernel, *deriv_kernel, *temp_hor, *temp_ver,  *horizontal_grad, *vertical_grad, *magnitude, *phase,*sup,*hyst,*edge,*subimage, *edges, *th_grad, *fh_grad, *tv_grad,
		      *fv_grad, *tmag, *tphase, *ft_sup, *t_sup, t_high,
		       t_low, *sorted, *ft_hyst, *t_edges, *ft_edges, *th_grad_gr,
		      *th_ver,*feature_g_ch1,*feature_g_ch2;	//  create pointer variable

		int height, width, k_width,comm_size, comm_rank,processor;// image height, width, and hernel width size 
		float sigma=atof(argv[2]); // to get sigma value from user command line 
		processor=atof(argv[3]);
		omp_set_num_threads(processor);

		// step 1. Create gaussian kernel and gaussian derivative kernel using sigma 

		gaussian(&g_kernel, &deriv_kernel, atof(argv[2]), &k_width); // atof function converts sigma
		
		// MPI initilization 
		MPI_Init(&argc,&argv);
		MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
		MPI_Comm_rank(MPI_COMM_WORLD, &comm_rank);

        	// step 2. At processor rank zero:  call function to read image and pass the funtion to the function 

	if(comm_rank==0)
	{

		// Print info for nodes 		
		printf("Processing %s using %d nodes \n", argv[1], comm_size);

		// time count start 

		gettimeofday(&start,NULL);		
			read_image_template(argv[1],&image,&width,&height);

		gettimeofday(&start_comp,NULL);		
			// dynamic array allocation 
			horizontal_grad = (float*)malloc(sizeof(float)*width*height);
			vertical_grad = (float*)malloc(sizeof(float)*width*height);
			magnitude = (float*)malloc(sizeof(float)*width*height);
			phase = (float*)malloc(sizeof(float)*width*height);
			sup = (float*)malloc(sizeof(float)*width*height);
			hyst = (float*)malloc(sizeof(float)*width*height);
			edges = (float*)malloc(sizeof(float)*width*height);

		
	}

		// Broadcast height, width, k_width, kernel & derivative 
		MPI_Bcast(&height, 1, MPI_INT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&width, 1, MPI_INT, 0, MPI_COMM_WORLD);

		// create image chunks 
		subimage = (float*)malloc(sizeof(float)*width*(height/comm_size));

		// distribuate image chunks between processors 
		MPI_Scatter(image, width*(height/comm_size), MPI_FLOAT, 
				subimage, width*(height/comm_size), 
				MPI_FLOAT, 0, MPI_COMM_WORLD);
				

		// 3. Horizontal and Vertical Gradients are computed using 1-D Gaussian kernel and a Gaussian derivative kernel

		//3.1	Horizontal Gradient:
		th_grad = sr_ghost(subimage, height/comm_size, width, floor(k_width/2), comm_size, comm_rank);
	   

		convoultion(th_grad, &temp_hor, g_kernel, height/comm_size, width, k_width, 1,comm_size, comm_rank); //smoothen first
		



		th_grad_gr = sr_ghost(temp_hor, height/comm_size, width, floor(k_width/2), comm_size, comm_rank);
		
		fh_grad = (float*)malloc(sizeof(float)*width*(height/comm_size));

		convoultion(th_grad_gr, &fh_grad, deriv_kernel, height/comm_size, width, 1, k_width,
				comm_size, comm_rank);
		
		MPI_Gather(fh_grad, width*(height/comm_size), MPI_FLOAT, horizontal_grad,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);



		
		//3.2 Vertcal gradient

		temp_ver = (float*)malloc(sizeof(float)*width*(height/comm_size));

		convoultion(th_grad, &temp_ver, g_kernel, height/comm_size, width, 1, k_width,
				comm_size, comm_rank);
		
		fv_grad = (float*) malloc(sizeof(float)*width*(height/comm_size));
		th_ver = sr_ghost(temp_ver, height/comm_size, width, floor(k_width/2), comm_size, comm_rank);
		convoultion(th_ver, &fv_grad, deriv_kernel, height/comm_size, width, k_width, 1,
				comm_size, comm_rank);

		MPI_Gather(fv_grad, width*(height/comm_size), MPI_FLOAT, vertical_grad,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

		
		// 4. Magnitude and Phase -> Input: Horizontal and Vertical Gradient, Output: Magnitude and Phase 


		tmag = (float*)malloc(sizeof(float)*(height/comm_size)*width);
		tphase = (float*)malloc(sizeof(float)*(height/comm_size)*width);

		gradient(fv_grad, fh_grad, &tmag, &tphase, height/comm_size, width);
		
		MPI_Gather(tmag, width*(height/comm_size), MPI_FLOAT, magnitude,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

		MPI_Gather(tphase, width*(height/comm_size), MPI_FLOAT, phase,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);


		//5. Suppress 	


		ft_sup = (float*)malloc(sizeof(float)*width*(height/comm_size));
		t_sup = sr_ghost(tmag, height/comm_size, width, 1, comm_size, comm_rank);
		suppress(t_sup, tphase, &ft_sup, height/comm_size, width, comm_rank, comm_size);	
	
		MPI_Gather(ft_sup, width*(height/comm_size), MPI_FLOAT, sup,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

		//6. hysteresis



	if(comm_rank==0){
			sorted = (float*)malloc(sizeof(float)*height*width);

			memcpy(sorted, sup, sizeof(float)*height*width);

			qsort(sorted, height*width, sizeof(float), floatcomp);

			t_high = *(sorted+(int)(0.95*height*width));
			t_low = t_high/5;
			
			free(sorted);
		}

		MPI_Bcast(&t_high, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);
		MPI_Bcast(&t_low, 1, MPI_FLOAT, 0, MPI_COMM_WORLD);

		hysteresis(ft_sup, &ft_hyst, height/comm_size, width, t_high, t_low);

		MPI_Gather(ft_hyst, width*(height/comm_size), MPI_FLOAT, hyst,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

		//7 Edge linking 


		t_edges = sr_ghost(ft_hyst, height/comm_size, width, 1, comm_size, comm_rank);

		find_edges(t_edges, &ft_edges, height/comm_size, width, comm_size, comm_rank);	
	
		MPI_Gather(ft_edges, width*(height/comm_size), MPI_FLOAT, edges,
				width*(height/comm_size), MPI_FLOAT, 0, MPI_COMM_WORLD);

		//8. Feature detection

		feature_g_ch1 = sr_ghost(fh_grad, height/comm_size, width, 1, comm_size, comm_rank);

		feature_g_ch2 = sr_ghost(fv_grad, height/comm_size, width, 1, comm_size, comm_rank);
		feature_detector (height/comm_size,width,feature_g_ch1,feature_g_ch2,comm_size, comm_rank);


		

		// 9. Write image 

	if(comm_rank==0)
{



		gettimeofday(&end_comp,NULL);

		//write_image_template("Temp_Horizontal.pgm", temp_hor, width, height);// temp hori image

		//write_image_template("Temp_Vertical.pgm", temp_ver, width, height);// temp ver image

		write_image_template("Horizontal_gradient.pgm", horizontal_grad, width, height); // hori gradient 
		write_image_template("Vertical_gradient.pgm", vertical_grad, width, height);// ver gradient image 

		write_image_template("Magitude.pgm", magnitude, width, height);// mag image 
		write_image_template("Phase.pgm", phase, width, height);// phase image 

		write_image_template("suppress.pgm", sup, width, height);

		write_image_template("hystersis.pgm",hyst, width, height);

		write_image_template("edges.pgm", edges, width, height);
		
		gettimeofday(&end,NULL);



		//print the reuqired time


		int time_comp=((end_comp.tv_sec*1000000+end_comp.tv_usec)-	(start_comp.tv_sec*1000000+start_comp.tv_usec))/1000;

		int time_end=((end.tv_sec*1000000+end.tv_usec)-			(start.tv_sec*1000000+start.tv_usec))/1000;

	printf("Image width: %d,Image Height: %d, Sigma: %f,threads: %d,Computation time (ms): %d\n, End to End time (ms): %d\n",width,height,sigma,processor,time_comp,time_end);
		/*
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
		free(edge);*/
}
		MPI_Finalize();
 
}

