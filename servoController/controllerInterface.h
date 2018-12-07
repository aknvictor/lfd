/*******************************************************************************************************************
*   Task-level robot programming for a LynxMotion AL5D robot arm
*
*   Interface file
*
********************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <time.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <termios.h>
#include <fcntl.h>
#include <ctype.h>
#include <iostream>
#include <vector>
#include <float.h>    

#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <signal.h>
#include <X11/Xlib.h>
#include <spnav.h>


/***************************************************************************************************************************

   Frame and vector classes to support task-level Robot Programming 
   ----------------------------------------------------------------

   Interface to the Frame and Vector classes and auxilliary friend functions required to implement a robot control program
   by specifying the position and orientation of objects and the robot manipulator using homogeneous transformations
   
   Author: David Vernon, Carnegie Mellon University Africa in Rwanda
   Date:   22/02/2017

****************************************************************************************************************************/

#ifndef CTRLLER_H
#define CTRLLER_H

class Vector {
public:
   Vector(double x=0, double y=0, double z=0, double w=1);
   void setValues(double x, double y, double z, double w);
   void getValues(double &x, double &y, double &z, double &w);
   void printVector()const;
   friend Vector operator+(Vector &a, Vector &b);
   friend double dotProduct(Vector &a, Vector &b);
private:
   double coefficient[4];
};


class Frame {
public:
   Frame();
   void printFrame()const;
   Frame        &operator*(Frame &h);
   Frame        &operator=(Frame &h);
   friend Frame trans(float x, float y, float z);
   friend Frame rotx(float theta);
   friend Frame roty(float theta);
   friend Frame rotz(float theta);
   friend Frame inv(Frame h);
   friend bool  move(Frame h);
private:
   double coefficient[4][4];
};


/***************************************************************************************************************************

   Robot Configuration 

****************************************************************************************************************************/
  
struct robotConfigurationDataType {
   char  com[13];                     // USB com port ID on windows ... need to check this in the control panel > device manager or use the SSC-32 servo sequencer utility
   int   baud;                        // Baud Rate
   int   speed;                       // Servo speed
   int   channel[6];                  // pin / channel numbers for each servo
   int   home[6];                     // setpoint for home position of each servo
   float degree[6];                   // pulse width per degree (in microseconds)
   int   effector_x;                  // x offset of end-effector w.r.t.  T5
   int   effector_y;                  // y offset of end-effector w.r.t.  T5
   int   effector_z;                  // z offset of end-effector w.r.t.  T5
   bool  lightweightWrist;            // one robot has a heavy duty wrist and it reverses the direction of the roll angle
};



#endif
/* function prototypes */

Vector operator+(Vector &a, Vector &b);
double dotProduct(Vector &a, Vector &b);
Frame trans(float x, float y, float z);
Frame rotx(float theta);
Frame roty(float theta);
Frame rotz(float theta);
Frame inv(Frame h);

bool move(Frame T5);

void grasp(int d);

/****************************************************************************************************************************
   Serial port interface 
   ---------------------

   Author: Victor Akinwande, Carnegie Mellon University Africa
   Modified by: David Vernon, Carnegie Mellon University Africa

****************************************************************************************************************************/

void goHome();
 
void sendToSerialPort(char *command);

void executeCommand(int channel, int pos, int speed);                           // single servo motor

void executeCommand(int * channel, int * pos, int speed, int number_of_servos); // multiple servo motors

bool getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d, int positions[]);

int  gotoPose(float x, float y, float z, float pitch, float roll);

int  pose_within_working_env(float x, float y, float z);

double degrees(double radians);

double radians(double degrees);

void sig(int s);

void readRobotConfigurationData(char filename[]);

float *scale_and_map(int m_x, int m_y, int m_z, int m_rx, int m_ry, int m_rz);

int timediff(struct timespec end, struct timespec start);