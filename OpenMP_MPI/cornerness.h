#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// The function provides the feature detection algothim from Cannny edge detector, The feature detection is implemented to put extra computational stress 

	void feature_detector(int height, int width, float *vertical, float *horizon,int comm_size, int comm_rank){
                // varibale declaration 
                float k = 0.04;  // constant for cornernees formula 
                int window_width = 7;// window size 
		float Ixx,Iyy,IxIy;
                int count,locatI,locatJ,countK,countM,locatI1[100000],locatJ1[100000];
		int locount=0;
		// dynamic memory allocation 
                float *cornerness = (float*)malloc(sizeof(float)*height*width);

                struct location {
					int locationI;
					int locationJ;
				};
		struct location *loc;


		int top,bottom;
		if (comm_rank==0){
					top=0;
					bottom=1;
				}
		else if  (comm_rank==comm_size-1)
			{
				top=1;
				bottom=0;

			} 
		else 
			{
			top=1;
			bottom=1;

			}
		



                // main algorithm implementation (using psudo code from lecture)
                for (int i=0; i<height; i++){
                    for (int j=0; j<width; j++){

                        Ixx = 0;
                        Iyy = 0;
                        IxIy = 0;

                        for(int k = -window_width/2; k < window_width/2 ; k++){

                            for(int m = -window_width/2; m < window_width/2 ; m++){
                                int offseti = i+k;
                                int offsetj = j+m;
                                if(offseti >= 0-top && offsetj >=0 && offseti < height+bottom && offsetj < width)/*boundary check*/ {

                                        Ixx = Ixx + pow(vertical[offseti+width+offsetj],2);
                                        Iyy = Iyy + pow(horizon[offseti+width+offsetj],2);
                                        IxIy = IxIy + vertical[offseti+width+offsetj] * horizon[offseti+width+offsetj];
                                  }
                                }                        
                            }  

                           *(cornerness+i*width+j)= Ixx*Iyy - pow(IxIy,2) - k* pow((Ixx+Iyy),2);
                    }
                }
                
			// to loop thorugh image 
                    int window_width1 = width/32;
                    int window_heigh = height/32;
		    loc=(struct location*)malloc (sizeof(struct location)*window_heigh*window_width1);
                       


                for (int i=0; i<height; i=i+window_heigh){
                    for(int j=0; j<width; j=j+window_width1){


                                locatI = i;
                                locatJ = j;



                        for (int k=0; k<window_heigh; k++){
                            for(int m=0; m<window_width; m++){ 
                                if(j+m >= 0 && i+k >=0 && j+m < width && i+k < height)

                                      

                                if (*(cornerness+(i+k)*width+(j+m))>*(cornerness+(locatI)*width+(locatJ))){
                                    
                                    locatI1[i] = i+k;
                                    locatJ1[j] = j+m;

                                }
                            }
                        }

	if (locount<window_width1*window_heigh && locount <1000)
			{
				loc[locount].locationI=locatI1[i];
				loc[locount].locationJ=locatJ1[j];
				locount+=1;
				



			}
                         

                    
                    }
                }
		
                
            } 
