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

// #define MAX(x, y) (((x) > (y)) ? (x) : (y))
// #define MIN(x, y) (((x) < (y)) ? (x) : (y))

char PORT[20];
char BAUD[6];
int ACTIVE_SERVOS_COUNT = 5;
int SPEED;
int MESASGE_LENGTH = 500;
int offset = 5;

int defaultPulseWidths[32] = {1800, 2200, 2000, 2000, 2000, 1500, 2000, 1500, 750, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500, 1500};
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
    if (numberOfServos > MAX_SERVOS)
    {
        char * message = new char[MESASGE_LENGTH];
        sprintf(message, "Maximum number of servos is %d", MAX_SERVOS);
        fail(message);
        delete[] message;
    }

    ACTIVE_SERVOS_COUNT = numberOfServos;

    char commandbuilder[COMMAND_SIZE] = "";
    char command[COMMAND_SIZE];

    for (int i = 0; i < ACTIVE_SERVOS_COUNT - 1; i++)
    {
        char temp[COMMAND_SIZE];

        //#channel Ppulsewidth Sspeed <carriage_return>

        sprintf(temp, "#%dP%d", i + offset, defaultPulseWidths[i + offset]);

        strcat(commandbuilder, temp);
        strcat(commandbuilder, " ");

        currentPulseWidths[i] = defaultPulseWidths[i];
    }
    char temp[COMMAND_SIZE];

    sprintf(temp, "S%d <CR>", SPEED);

    strcat(commandbuilder, temp);

    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);
    // printf(commandbuilder);
    system(command);
    sleep(DEFAULT_SLEEP_TIME);
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

    printf("going home.. \n");
    for (int i = 0; i < ACTIVE_SERVOS_COUNT - 1; i++)
    {
        goServoHome(i);
    }

    printf("done.. \n");
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

    sprintf(commandbuilder, "#%dP%dS%d", index, defaultPulseWidths[index], SPEED);
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

    sprintf(commandbuilder, "#%dP%dS%d <CR>", index, pw, SPEED);
    sprintf(command, "echo \"%s\" > %s", commandbuilder, PORT);

    system(command);
    sleep(DEFAULT_SLEEP_TIME);
    //sleep(getTimeNeeded(index, pw, SPEED));

    currentPulseWidths[index] = pw;
}

void executeCommand(int channel, int pos, int speed)
{

    channel = channel + offset;

    SPEED = speed;
    char command[200];

    //setServoPW(channel, pos);

    sprintf(command, "#%dP%dS%d <CR>", channel, pos, speed);
    execute(command);
}

/**
 * Set specific command
 * @param command, string to send to device - 
 * #channel Ppulsewidth Sspeed <carriage_return>
 */
void execute(char *command)
{

    if (sizeof(command) < 6)
    {
        char *message = new char[MESASGE_LENGTH];
        sprintf(message, "Invalid command. Should be of size %d at least", 6);
        fail(message);

        delete[] message;
        
    }
    else
    {
        char execcommand[sizeof(command) + 20];

        sprintf(execcommand, "echo \"%s\" > %s", command, PORT);

        system(execcommand);
    }

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

// method to recieve catesian coordinates and translate to joint positions (pulse widths)

//inverse kinematics routine

//inverse kinematics code authored by Prof. Vernon

/* Arm dimensions (mm). Standard AL5D arm */
#define D1 70       // Base height to X/Y plane
#define A3 146.0    // Shoulder-to-elbow "bone"
#define A4 187.0    // Elbow-to-wrist "bone"
#define EZ 100      // Gripper length
#define DEG_PW 0.09 //degrees to pulse width conversation factor

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

int *getJointPositions(float x, float y, float z, float pitch_angle_d, float roll_angle_d)
{

    int *positions = (int*) malloc(sizeof(int) * 6);

    float hum_sq;
    float uln_sq;
    float wri_roll_angle_d;
    float bas_pos;
    float shl_pos;
    float elb_pos;
    float wri_pitch_pos;
    float wri_roll_pos;

    hum_sq = A3 * A3;
    uln_sq = A4 * A4;

    /* the servo angles corresponding to joint angles of zero */

    // zeroOffset[1] = 90;
    // zeroOffset[2] = 140;
    // zeroOffset[3] = 90;
    // zeroOffset[4] = 90;
    // zeroOffset[5] = 90;
    // zeroOffset[6] = 0;

    /*VA: the servo angles corresponding to home joint angles */

    zeroOffset[1] = 135;
    zeroOffset[2] = 180;
    zeroOffset[3] = 135;
    zeroOffset[4] = 135;
    zeroOffset[5] = 135;
    zeroOffset[6] = 0;

    //grip angle in radians for use in calculations
    //float pitch_angle_r = radians(pitch_angle_d);
    //float roll_angle_r = radians(roll_angle_d);

    // Base angle and radial distance from x,y coordinates
    float bas_angle_r = atan2(x, y);
    float bas_angle_d = degrees(bas_angle_r);

    float rdist = sqrt((x * x) + (y * y));

    // rdist is y coordinate for the arm
    y = rdist; //DV BAD PRACTICE IN ORIGINSAL CODE: OVERWRITING A PARAMETER!

    // Wrist position
    float wrist_z = z - D1;
    float wrist_y = y;

    // Shoulder to wrist distance (AKA sw)
    float s_w = (wrist_z * wrist_z) + (wrist_y * wrist_y);
    float s_w_sqrt = sqrt(s_w);

    // s_w angle to ground
    float a1 = atan2(wrist_z, wrist_y);

    // s_w angle to A3
    float a2 = acos(((hum_sq - uln_sq) + s_w) / (2 * A3 * s_w_sqrt));

    // Shoulder angle
    float shl_angle_r = a1 + a2;
    // If result is NAN or Infinity, the desired arm position is not possible

    // printf("\n Entering IK \n");

    if (std::isnan(shl_angle_r) || std::isinf(shl_angle_r))
        return positions;
    float shl_angle_d = degrees(shl_angle_r);

    //float a1_d = degrees(a1);
    //float a2_d = degrees(a2);

    // Elbow angle

    float elb_angle_r = acos((hum_sq + uln_sq - s_w) / (2 * A3 * A4));
    // If result is NAN or Infinity, the desired arm position is not possible
    if (std::isnan(elb_angle_r) || std::isinf(elb_angle_r))
        return positions;

    float elb_angle_d = degrees(elb_angle_r);
    float elb_angle_dn = -(180.0 - elb_angle_d);

    // Wrist angles
    float wri_pitch_angle_d = (pitch_angle_d - elb_angle_dn) - shl_angle_d;

    if (((int)pitch_angle_d == -90) || ((int)pitch_angle_d == 90))
    {

        /* special case: we can adjust the required roll to compensate for the base rotation */

        wri_roll_angle_d = roll_angle_d - bas_angle_d;
    }
    else
    {
        wri_roll_angle_d = roll_angle_d;
    }

    // Calculate servo angles
    // Calc relative to servo midpoint to allow compensation for servo alignment
    /*
    bas_pos = BAS_MID + degrees(bas_angle_r);
    shl_pos = SHL_MID + (shl_angle_d - 90.0);  
    elb_pos = ELB_MID - (elb_angle_d - 90.0); 
    wri_pitch_pos = WRI_MID + wri_pitch_angle_d;
    wri_roll_pos  = WRO_MID + wri_roll_angle_d;
    */

    bas_pos = bas_angle_d + zeroOffset[1];
    shl_pos = (shl_angle_d - 90.0) + zeroOffset[2];
    elb_pos = -(elb_angle_d - 90.0) + zeroOffset[3];
    wri_pitch_pos = wri_pitch_angle_d + zeroOffset[4];
    wri_roll_pos = wri_roll_angle_d + zeroOffset[5];

//#define DEBUG  // we use conditional compilation to avoid compiling unused code
#ifdef DEBUG
    printf("X: ");
    printf(x);
    printf("  Y: ");
    printf(y); // DV THIS HAS BEEN OVERWRITTEN IN ORIGINAL CODE
    printf("  Z: ");
    printf(z);
    printf("  pitch: ");
    printf(pitch_angle_d);
    printf("  roll: ");
    printf(roll_angle_d);
    printfln();
    printf("Base Pos: ");
    printf(bas_pos);
    printf("  Shld Pos: ");
    printf(shl_pos);
    printf("  Elbw Pos: ");
    printf(elb_pos);
    printf("  Pitch Pos: ");
    printfln(wri_pitch_pos);
    printf("  Roll Pos: ");
    printfln(wri_roll_pos);
    printf("bas angle: ");
    printf(degrees(bas_angle_r));
    printf("  shl angle: ");
    printf(shl_angle_d);
    printf("  elb angle: ");
    printf(elb_angle_d);
    printf("  Pitch d: ");
    printfln(wri_pitch_angle_d);
    printf("  Roll d: ");
    printfln(wri_roll_angle_d);
    printfln();

#endif

    //VA: convert positions in degrees to pulse width (0.09 degrees per unit pulse width)

    positions[0] = (int)(bas_pos / DEG_PW);
    positions[1] = (int)(shl_pos / DEG_PW);
    positions[2] = (int)(elb_pos / DEG_PW);
    positions[3] = (int)(wri_pitch_pos / DEG_PW);
    positions[4] = (int)(wri_roll_pos / DEG_PW);

    // printf("\n Exiting IK \n");

    return positions;
}

// 0 (open) - 60(close)
void grasp(int d)
{

    /* the grippers are approximately 30mm apart when open at servo angle of 0 
   * and 0mm apart when closed at servo angle of 140 (500 PW - 1555 PW )
   * Thus, we close to approximately d mm with a servo angle of 140-4.7d 
   * or PW of 1553-35.2d
   */

    int pw;
    pw = (int)(500 + 35.17 * (float)d);

    // int angle;
    //angle = (int)(150 - 4.6 * (float)d);

    executeCommand(5, pw, SPEED);
}

int gotoPose(float x, float y, float z, float pitch, float roll)
{
    // printf("Cartesian pose: \n  %f %f %f %f %f \n", x, y, z, pitch, roll);

    int *poss = getJointPositions(x, y, z, pitch, roll);

    if (poss[0] != 0)
    {
        
        executeCommand(0, MIN(MAX_PW, MAX(MIN_PW, poss[0])), SPEED);
        executeCommand(1, MIN(MAX_PW, MAX(MIN_PW, poss[1])), SPEED);
        executeCommand(2, MIN(MAX_PW, MAX(MIN_PW, poss[2])), SPEED);
        executeCommand(3, poss[3], SPEED);
        executeCommand(4, poss[4], SPEED);

        // printf("Joint pos: \n %d %d %d %d %d \n", poss[0], poss[1],  poss[2], poss[3], poss[4]);
        return 1;
    }
    else
    {
        printf("Joint pos: Unatainable \n");
        return 0;
    }
}