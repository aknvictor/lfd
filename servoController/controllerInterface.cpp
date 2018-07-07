//controllerinterface.c

//todo
//validation
//memory

#include "controllerInterface.h"

//Helper function prototypes

void fail(char *message);
int getTimeNeeded(int idx, int destination, int speed);
int getDistance(int idx, int destination);
int getPW(int idx);

#define DEFAULT_SLEEP_TIME 1
#define COMMAND_SIZE 200
#define MAX_SERVOS 32
#define MIN_PW 750  //lowest pulse width
#define MAX_PW 2250 //highest pulsewidth

//working environment 

#define MAX_X 150
#define MAX_Y 200
#define MAX_Z 250
#define MIN_X -150
#define MIN_Y 100
#define MIN_Z 90

#define MAX_R 0
#define MIN_R -120

// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))

char PORT[20];
char BAUD[6];
int ACTIVE_SERVOS_COUNT = 5;
int SPEED;
int MESASGE_LENGTH = 500;
int offset = 0; //pin starting point. Ideally the first servo would be connected to pin 0

int defaultPulseWidths[32] = {1460, 1450, 1490, 1430, 1440, 1800, 2200, 2000, 2000, 2000, 1500, 2000, 1500, 750, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
int currentPulseWidths[32] = {1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};

/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
 *  @param speed - servo speed
*/
void initializeControllerWithSpeed(char *port, char *baudrate, int speed)
{
    strcpy(PORT, port);
    strcpy(BAUD, baudrate);
    SPEED = speed;
}

/**
 * Set global speed of all servos
 */
void setSpeed(int speed)
{
    SPEED = speed;
}

/** 
 *  intialize the port and baurate global variables
 *  @param port - port number of the USB serial
 *  @param baudrate - baud rate of the USB serial
*/
void initializeController(char *port, char *baudrate)
{
    strcpy(PORT, port);
    strcpy(BAUD, baudrate);
}

/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome(int numberOfServos)
{
    printf("%s\n", "going home");

    if (numberOfServos > MAX_SERVOS)
    {
        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Maximum number of servos is %d", MAX_SERVOS);
        fail(message);
        delete[] message;
    }

    ACTIVE_SERVOS_COUNT = numberOfServos;

    int * channels = new int[ACTIVE_SERVOS_COUNT];
    int * pos = new int[ACTIVE_SERVOS_COUNT];
    
    for (int i = 0; i < ACTIVE_SERVOS_COUNT; i++)
    {
        channels[i] = i + offset;
        pos[i] = defaultPulseWidths[i+ offset];
    }

    executeCommand(channels, pos, SPEED, ACTIVE_SERVOS_COUNT);

    delete [] channels;
    delete [] pos;

    grasp(0);

}

/** 
 * set the positions of the servos connected to 
 * the controller to their default positions
 * @param numberOfServos 
 */
void goHome2(int numberOfServos)
{
    ACTIVE_SERVOS_COUNT = numberOfServos;
    if (numberOfServos > MAX_SERVOS)
    {
        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Maximum number of servos is %d", MAX_SERVOS);
        fail(message);
        delete[] message;
    }

    ACTIVE_SERVOS_COUNT = numberOfServos;

    for (int i = 0; i < ACTIVE_SERVOS_COUNT ; i++)
    {
        goServoHome(i);
    }

}

/** 
 * set the position of the specified servo to its default position
 * @param index
 */
void goServoHome(int index)
{
    if (index >= ACTIVE_SERVOS_COUNT)
    {
        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Servo not active");
        fail(message);
        delete[] message;
    }

    char commandbuilder[COMMAND_SIZE];
    char command[COMMAND_SIZE];

    //#channel Ppulsewidth Sspeed <carriage_return>

    index = index + offset;

    //printf("\n #%dP%dS%d <CR> \n",  index, defaultPulseWidths[index], SPEED);

    sprintf(commandbuilder, " #%dP%dS%d", index, defaultPulseWidths[index], SPEED);
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);

    system(command);
    currentPulseWidths[index] = defaultPulseWidths[index];

    sleep(DEFAULT_SLEEP_TIME);
}

/**
 * Set specific servo pulse width
 * @param index, index of the servo
 * @param pw, pulse width
 */
void setServoPW(int index, int pw)
{
    if (index >= ACTIVE_SERVOS_COUNT)
    {
        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Servo not active");
        fail(message);
        delete[] message;
    }

    index = index + offset;

    if (pw < MIN_PW || pw > MAX_PW)
    {

        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Pulse width not in range");
        fail(message);

        delete[] message;
        
    }

    char commandbuilder[COMMAND_SIZE];
    char command[COMMAND_SIZE];

    //#channel Ppulsewidth Sspeed <carriage_return>

    sprintf(commandbuilder, " #%dP%dS%d <CR>", index, pw, SPEED);
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);

    currentPulseWidths[index] = pw;

    system(command);
    sleep(DEFAULT_SLEEP_TIME);
    //sleep(getTimeNeeded(index, pw, SPEED));

}

void executeCommand(int * channel, int * pos, int speed, int size)
{
    char commandbuilder[COMMAND_SIZE] = {0};
    
    for(int i =0; i< size; i++)
    {

        char temp[COMMAND_SIZE] = {0};

        if(i == 0)
        sprintf(temp, " #%dP%d", channel[i], pos[i]);
        else
        sprintf(temp, " #%dP%dS%d", channel[i], pos[i], speed);

        strcat(commandbuilder, temp);
        strcat(commandbuilder, " ");

        currentPulseWidths[i] = pos[i];

    }

    execute(commandbuilder);

}

void executeCommand(int channel, int pos, int speed)
{

    channel = channel + offset;

    SPEED = speed;
    char command[200];

    //setServoPW(channel, pos);

    sprintf(command, " #%dP%dS%d <CR>", channel, pos, speed);

    execute(command);
}

/**
 * Set specific command
 * @param command, string to send to device - 
 * #channel Ppulsewidth Sspeed <carriage_return>
 */
void execute(char *command)
{

    if (strlen(command) < 6)
    {
        char *message = new char[MESASGE_LENGTH];
        sprintf(message, "Invalid command. Should be of size %d at least", 6);
        fail(message);

        delete[] message;
        
    }
    else
    {
        char execcommand[COMMAND_SIZE];

        sprintf(execcommand, "echo \"%s\" > %s", command, PORT);

        //printf("%s\n", execcommand);
        
        system(execcommand);
    }

    // sleep(DEFAULT_SLEEP_TIME);f

}

//helper methods

/**
 * Print failure message and return
 * @param message, output message
 */
void fail(char *message)
{
    printf("%s\n", message);
    exit(1);
}

/**
 * Gets the time needed to complete the movement
 * Time = Distance / Speed
 * @param idx, index of the servo
 * @param destination, destination pulse width
 * @param speed, the speed being used
 */
int getTimeNeeded(int idx, int destination, int speed)
{
    int neededTime;
    int distance = getDistance(idx, destination);
    neededTime = distance / speed;
    return neededTime;
}

/**
 * Gets the distance between the current position and the destination
 * @param idx, index of the servo
 * @param destination, destination pulse width
 */
int getDistance(int idx, int destination)
{
    int distance = abs(getPW(idx) - destination);
    return distance;
}

/**
 * Get pulse width of specified servo
 * @param idx, index of the servo
 */
int getPW(int idx)
{
    return currentPulseWidths[idx];
}

// method to recieve cartesian coordinates and translate to joint positions (pulse widths)

//inverse kinematics routine

//inverse kinematics code authored by Prof. Vernon

/* Arm dimensions (mm). Standard AL5D arm */
#define D1 70       // Base height to X/Y plane
#define A3 146.0    // Shoulder-to-elbow "bone"
#define A4 187.0    // Elbow-to-wrist "bone"
#define EZ 100      // Gripper length
#define DEG_PW 0.09 //degrees to pulse width conversion factor

int zeroOffset[7];

float degrees(float radians)
{
    float degrees = radians * 180.0 / M_PI;
    return degrees;
}

float radians(float degrees)
{
    float radians = degrees / (180.0 / M_PI);
    return radians;
}

bool getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d, int positions[])
{

    int zeroOffset[6];  
    float pw_per_degree[6];

    bool debug = false; // David Vernon

    if (debug) printf("getJointPositions(): %4.1f %4.1f %4.1f %4.1f %4.1f\n", x, y, z, pitch_angle_d, roll_angle_d);

    // int *positions = (int*) malloc(sizeof(int) * 6); // David Vernon  ... commented out

    double hum_sq;
    double uln_sq;
    double wri_roll_angle_d;
    double bas_pos;
    double shl_pos;
    double elb_pos;
    double wri_pitch_pos;
    double wri_roll_pos;

    hum_sq = A3 * A3;
    uln_sq = A4 * A4;

    /* Set the servo angles corresponding to joint angles of zero */
                                                                
    /* need to read these value from a configuration file to allow calibration of different robots                                  */
    /* Also need to calibrate the degree-to-pulse-width value DEG_PW, possibly on a servo by servo basis                            */
    /* finally, need to move this out of this function so that the values only computed once and the array is passed as an argument */
  
    /* from calibration */

    pw_per_degree[0] = (float) 10.1;
    pw_per_degree[1] = (float) 8.9;
    pw_per_degree[2] = (float) 9.1;
    pw_per_degree[3] = (float) 10.0;
    pw_per_degree[4] = (float) 10.7;

    zeroOffset[0] = (int) (1460 / pw_per_degree[0]);
    zeroOffset[1] = (int) (1450 / pw_per_degree[1]);
    zeroOffset[2] = (int) (1490 / pw_per_degree[2]);
    zeroOffset[3] = (int) (1430 / pw_per_degree[3]);
    zeroOffset[4] = (int) (1440 / pw_per_degree[4]);

    // Base angle and radial distance from x,y coordinates
    double bas_angle_r = atan2(x, y);
    double bas_angle_d = degrees(bas_angle_r);

    double rdist = sqrt((x * x) + (y * y));

    // rdist is y coordinate for the arm
    y = (float) rdist; //DV BAD PRACTICE IN ORIGINAL CODE: OVERWRITING A PARAMETER!

    // Wrist position
    double wrist_z = z - D1;
    double wrist_y = y;

    // Shoulder to wrist distance (AKA sw)
    double s_w = (wrist_z * wrist_z) + (wrist_y * wrist_y);
    double s_w_sqrt = sqrt(s_w);

    // s_w angle to ground
    double a1 = atan2(wrist_z, wrist_y);

    // s_w angle to A3
    double a2 = (float) acos(((hum_sq - uln_sq) + s_w) / (2 * A3 * s_w_sqrt));  // David Vernon ... cast to float

    // Shoulder angle
    double shl_angle_r = a1 + a2;
    // If result is NAN or Infinity, the desired arm position is not possible

    if (std::isnan(shl_angle_r) || std::isinf(shl_angle_r))
        return false;          // David Vernon ... not a valid pose 

    double shl_angle_d = degrees(shl_angle_r);
    
    // Elbow angle

    double elb_angle_r = acos((hum_sq + uln_sq - s_w) / (2 * A3 * A4)); // David Vernon ... cast to float
    // If result is NAN or Infinity, the desired arm position is not possible
    if (std::isnan(elb_angle_r) || std::isinf(elb_angle_r)) 
        return false;          // David Vernon ... not a valid pose 

    double elb_angle_d = degrees(elb_angle_r);
    double elb_angle_dn = -((double)180.0 - elb_angle_d);  

    // Wrist angles
    //-------------
    /* Changed original solution by adding 90 degrees to rotation about y axis (pitch) and about z axis (roll) to ensure that              */
    /* the z axis of the wrist (i.e. the approach vector) is aligned with the z axis of the frame in base of the robot, i.e. frame Z       */  
    /* and the y axis of the wrist (i.e. the orientation vector) is aligned with the y axis of frame Z                                     */
    /* Thus, if T5 is pure translation the the wrist is pointing vertically upward and the plane of the gripper is aligned with the y axis */ 
    /* David Vernon 24 April 2018                                                                                                          */
    
    //float wri_pitch_angle_d = (pitch_angle_d - elb_angle_dn) - shl_angle_d; // original code
    double wri_pitch_angle_d = (pitch_angle_d - elb_angle_dn) - shl_angle_d + 90;

    // if (((int)pitch_angle_d == -90) || ((int)pitch_angle_d == 90)) // original code
    if (((int)pitch_angle_d == -180) || ((int)pitch_angle_d == 180))  // directed vertically up or down
    {

        /* special case: we can adjust the required roll to compensate for the base rotation */

        // wri_roll_angle_d = roll_angle_d - bas_angle_d;   // original code
        wri_roll_angle_d = roll_angle_d - bas_angle_d + 90; // gripper orientation aligned with y axis
    }
    else
    {
        // should really throw an exception here because this isn't the correct
        wri_roll_angle_d = roll_angle_d;
    }

    // Calculate servo angles

    bas_pos       = bas_angle_d                   + zeroOffset[0];
    shl_pos       = (shl_angle_d  - (float) 90.0) + zeroOffset[1];  
    elb_pos       = -(elb_angle_d - (float) 90.0) + zeroOffset[2];  
    wri_pitch_pos = wri_pitch_angle_d             + zeroOffset[3];     
    // wri_roll_pos  = wri_roll_angle_d              + zeroOffset[4]; // David Vernon ... roll servor rotates in reverse
    wri_roll_pos  = - wri_roll_angle_d            + zeroOffset[4];    //                  with the medium duty wrist attachment
    
    if (debug) {
       printf("X: %4.1f ",x);
       printf("Y: %4.1f ",y); // DV THIS HAS BEEN OVERWRITTEN IN ORIGINAL CODE
       printf("Z: %4.1f ",z);
       printf("pitch: %4.1f ",pitch_angle_d);
       printf("roll:  %4.1f \n",roll_angle_d);
       printf("Base Pos: %4.1f ",bas_pos);
       printf("Shld Pos: %4.1f ",shl_pos);
       printf("Elbw Pos: %4.1f ",elb_pos);
       printf("Pitch Pos: %4.1f ",wri_pitch_pos);
       printf("Roll Pos: %4.1f \n",wri_roll_pos);
       printf("bas angle: %4.1f ",degrees(bas_angle_r));
       printf("shl angle: %4.1f ",shl_angle_d);
       printf("elb angle: %4.1f ",elb_angle_d);
       printf("Pitch d: %4.1f ",wri_pitch_angle_d);
       printf("Roll  d: %4.1f ",wri_roll_angle_d);
       printf("\n");
    }

    //convert angles in degrees to pulse width based on calibration data
    pw_per_degree[0] = (float) 10.1;
    pw_per_degree[1] = (float)  8.9;
    pw_per_degree[2] = (float)  9.1;
    pw_per_degree[3] = (float) 10.0;
    pw_per_degree[4] = (float) 10.7;

    positions[0] = (int)(bas_pos       * pw_per_degree[0]);
    positions[1] = (int)(shl_pos       * pw_per_degree[1]);
    positions[2] = (int)(elb_pos       * pw_per_degree[2]);
    positions[3] = (int)(wri_pitch_pos * pw_per_degree[3]);
    positions[4] = (int)(wri_roll_pos  * pw_per_degree[4]);

    return true; // David Vernon ... valid pose
}


void grasp(int d) // d is distance between finger tips:  0 <= d <= 30 mm
{

  /* the grippers are approximately 30mm apart (i.e. fully open) when open at servo PW of 860 
   * and 0mm apart (i.e. closed) when closed at servo PW of 2500
   * Thus, we close to approximately d mm with a PW of 2500 - d* (2500-860) / 30
   */

    int pw;
    pw = (int) (2500 - (float (d) * (2500-860) / 30 ));

    executeCommand(5, pw, SPEED *2);  // gripper servo is on pin 5
}

int pose_within_working_env(float x, float y, float z, float r)
{
    int w = (int)x <= MAX_X && (int) x > MIN_X && (int)y <= MAX_Y && (int) y > MIN_Y && (int)z <= MAX_Z && (int) z > MIN_Z && (int)r <= MAX_R && (int) r > MIN_R; 

    if(w)  return 1;
    else return 0;
}

int gotoPose(float x, float y, float z, float pitch, float roll)
{
    if(!pose_within_working_env(x, y, z, roll)) return 0;
    printf("Cartesian pose: \n  %f %f %f %f %f \n", x, y, z, pitch, roll);

    int * poss = new int [6];

    bool status = getJointPositions(x, y, z, pitch, roll, poss );

    if (status)
    {
        int * channels = new int [5];
        int * pos = new int [5];

        channels[0] = 0 + offset;
        channels[1] = 1 + offset;
        channels[2] = 2 + offset;
        channels[3] = 3 + offset;
        channels[4] = 4 + offset;

        pos[0] = MIN(MAX_PW, MAX(MIN_PW, poss[0]));
        pos[1] = MIN(MAX_PW, MAX(MIN_PW, poss[1]));
        pos[2] = MIN(MAX_PW, MAX(MIN_PW, poss[2]));
        pos[3] = poss[3];
        pos[4] = poss[4];
        

        executeCommand(0, poss[0], SPEED);
        executeCommand(1, poss[1], SPEED);
        executeCommand(2, poss[2], SPEED);
        executeCommand(3, poss[3], SPEED);
        executeCommand(4, poss[4], SPEED);

        // executeCommand(channels, pos, SPEED, 5);

        // printf("Joint pos: \n %d %d %d %d %d \n", poss[0], poss[1],  poss[2], poss[3], poss[4]);
    }
    else
    {
        printf("Joint pos: Unatainable \n");
    }

    delete [] poss;
    return status;

}