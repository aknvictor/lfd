/* 
  Example use of openCV to track an object using colour segmentation and the Kalman filter
  -----------------------------------------------------------------------------------------
  
  The images are input from the camera

  The minimum and maximum hue values of the object to be tracked are read from from the input file trackingKalmanFilterInput.txt

  It is assumed that the input file is located in a data directory given by the path ../data/ 
  defined relative to the location of executable for this application.

  Adapted from the example provided here http://www.robot-home.it/blog/en/software/ball-tracker-con-filtro-di-kalman/                         
  and code here https://github.com/Myzhar/simple-opencv-kalman-tracker/blob/8df8fb04bd06ef0be06c9b68a79fda6edf3efa1a/source/opencv-kalman.cpp 

  (This is the application file: it contains the client code that calls dedicated functions to implement the application.
  The code for these functions is defined in the implementation file. The functions are declared in the interface file.)

  David Vernon
  24 November 2017
*/

#include "trackingKalmanFilter.h"

int main() {

   int end_of_file;
   bool debug = true;
   int min_hue;
   int max_hue;
   int min_sat;
   int max_sat;

   FILE *fp_in; 

   printf("Example use of openCV to track an object using the Kalman filter\n\n");

   if ((fp_in = fopen("trackingKalmanFilterInput.txt","r")) == 0) {
	  printf("Error can't open input file trackingKalmanFilterInput.txt\n");
     prompt_and_exit(1);
   }

    end_of_file = fscanf(fp_in, "%d %d %d %d", &min_hue, &max_hue, &min_sat, &max_sat);

   if (end_of_file != EOF) { 
      trackingKalmanFilter(min_hue, max_hue, min_sat, max_sat);  
   }

   fclose(fp_in); 
  
   return 0;
}
