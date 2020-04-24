#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


// The function provides the feature detection algothim from Cannny edge detector, The feature detection is implemented to put extra computational stress 

	

	

void feature_detector(int height, int width, float *vertical, float *horizon){
                // varibale declaration 
                float k = 0.04;  // constant for cornernees formula 
                int window_width = 7;// window size 
		float Ixx,Iyy,IxIy;
                int count,locatI,locatJ,countK,countM,locatI1[100000],locatJ1[100000];

		// dynamic memory allocation 
                float *cornerness = (float*)malloc(sizeof(float)*height*width);
                float *C_hor = (float*)malloc(sizeof(float)*height*width);
                float *C_ver = (float*)malloc(sizeof(float)*height*width);
                

                struct location *loc;

                // copy horizontal gradient & vertical gradient to C_hor & C_ver 
                memcpy(C_hor, horizon, sizeof(float)*height*width); 
                memcpy(C_ver, vertical, sizeof(float)*height*width);    


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
                                if(offseti >= 0 && offsetj >=0 && offseti < height && offsetj < width)/*boundary check*/ {

                                        Ixx = Ixx + pow(C_ver[offseti+width+offsetj],2);
                                        Iyy = Iyy + pow(C_hor[offseti+width+offsetj],2);
                                        IxIy = IxIy + C_ver[offseti+width+offsetj] * C_hor[offseti+width+offsetj];
                                  }
                                }                        
                            }  

                           *(cornerness+i*width+j)= Ixx*Iyy - pow(IxIy,2) - k* pow((Ixx+Iyy),2);
                    }
                }
                
			// to loop thorugh image 
                    int window_width1 = width/32;
                    int window_heigh = height/32;

                       
                    int locount = 0;

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
                         

                    
                    }
                }
		
                // free memory 
                free(C_hor);
                free(C_ver);
            } 
