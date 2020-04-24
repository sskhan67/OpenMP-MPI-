#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


float *sr_ghost(float *image, int height, int width, int ngr, int comm_size, int comm_rank){
	MPI_Status status;
	float *output;
	if(comm_rank==0 || comm_rank==(comm_size-1))
		output = (float*)malloc(sizeof(float)*width*(height + ngr));
	else
		output = (float*)malloc(sizeof(float)*width*(height + 2*ngr));
	if(comm_rank==0){
		// send bottom and receive top from rank+1
		MPI_Sendrecv(image+width*(height-ngr), ngr*width, MPI_FLOAT, comm_rank+1, comm_rank,
				output+width*height, ngr*width, MPI_FLOAT, comm_rank+1, comm_rank+1, 
				MPI_COMM_WORLD, &status);
	
		memcpy(output, image, sizeof(float)*width*height);

		return output;
	}
	else if(comm_rank==(comm_size-1)){
		// sent top and receive bottom from rank-1
		MPI_Sendrecv(image, ngr*width, MPI_FLOAT, comm_rank-1, comm_rank,
				output, ngr*width, MPI_FLOAT, comm_rank-1, comm_rank-1, MPI_COMM_WORLD, 
				&status);
	
		memcpy(output+ngr*width, image, sizeof(float)*width*height);
		return output+ngr*width;
	}
	else{
		//send top and receive top from rank+1	
		MPI_Sendrecv(image, ngr*width, MPI_FLOAT, comm_rank-1, comm_rank,
				output, ngr*width, MPI_FLOAT, comm_rank-1, comm_rank-1, MPI_COMM_WORLD, 
				&status);
		// send bottom and receive top from rank+1
		MPI_Sendrecv(image+width*(height-ngr), ngr*width, MPI_FLOAT, comm_rank+1, comm_rank,
				output+width*(height+ngr), ngr*width, MPI_FLOAT, comm_rank+1, comm_rank+1, 
				MPI_COMM_WORLD, &status);
		
		memcpy(output+ngr*width, image, sizeof(float)*width*height);
		return output+ngr*width;
	}
}

