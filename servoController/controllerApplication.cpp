#include "controllerInterface.h"

int main(int argc, char ** argv)
{

    char PORT[13] = "/dev/ttyUSB0"; // USB Port ID on windows - "\\\\.\\COM9" on linux "/dev/ttyUSB0;
    char BAUD[7] = "9600";          // Baud Rate
    int speed = 300;                // Servo speed

    int x = 0;
    int y = 200;
    int z = 120;
    int pitch = -180;
    int roll = -90;
    int graspVal = 60;

    readRobotConfigurationData("../applicationControl/robotConfig.txt");

    goHome();

    printf("%d", gotoPose(x, y, z , pitch, roll));
    

    spnav_event sev;
    signal(SIGINT, sig);

    if (spnav_open() == -1)
    {
        fprintf(stderr, "failed to connect to the space navigator daemon\n");
        return 1;
    }


    while (spnav_wait_event(&sev))
    {
        if (sev.type == SPNAV_EVENT_MOTION)
            {
                float *poseDelta = scale_and_map(sev.motion.x, sev.motion.y, sev.motion.z, sev.motion.rx, sev.motion.ry, sev.motion.rz);
                gotoPose(poseDelta[0] + x, poseDelta[1] + y, poseDelta[2] + z , poseDelta[3] + pitch, poseDelta[4] + roll)
            }
        else if (sev.type == SPNAV_EVENT_BUTTON){
            return 0;
            spnav_close();
        } 

    }
    
    spnav_close();
    return 0;

}