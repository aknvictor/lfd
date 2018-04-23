//controllerinterface.h

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


#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/video/video.hpp>

#include <signal.h>
#include <X11/Xlib.h>
#include <spnav.h>

/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
 *  @param speed - servo speed
*/
void initializeControllerWithSpeed(char *port, char *baudrate, int speed);


/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
*/
void initializeController(char *port, char *baudrate);

/**
 * Set global speed of all servos
 */
void setSpeed(int speed);

/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome(int numberOfServos);

/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome2(int numberOfServos);


/** 
 * set the position of the specified servo to its default position
 * @param index
 */
void goServoHome(int index);

/**
 * Set specific servo pulse width
 * @param index, index of the servo
 * @param pw, pulse width
 */

void setServoPW(int index, int pw);

/**
 * Set specific command
 * @param command, string to send to device - 
 * #channel Ppulsewidth Sspeed <carriage_return>
 */
void execute(char *command);

/** 
 * execute the appropriate command on the device 
 * given the channel, pulse width and speed
 * @param channel, pos, speed 
 */
void executeCommand(int channel, int pos, int speed);

/** 
 * execute the appropraite command on the device 
 * given the channel(s), pulse width(s) and speed
 * @param channel(s), pos(s), speed 
 */
void executeCommand(int * channel, int * pos, int speed, int size);


/** 
 * compute the pulse width of each servo corresponding
 *  the caetesian pose specified 
 * @param x, y, z, pitch_angle_d, roll_angle_d
 */
int *getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d);


/** 
 * goto to a specifed pose in cartesian frame of reference
 * @param x, y, z, pitch, roll
 */
int gotoPose(float x, float y, float z, float pitch, float roll);


/** 
 * open or close the gripper
 * @param d - 0 is for open and 60 is for close
 */
void grasp(int d);

/** 
 *
 * check if the specified pose in cartesian frame of reference is within
 * the working enviroment 
 * @param x, y, z
 */
int pose_within_working_env(float x, float y, float z);


